#include <pwcpp/filter/app_builder.h>

#include <systemd/sd-daemon.h>

#include <condition_variable>
#include <iostream>
#include <thread>

#include <boost/asio.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <logging/logger.h>

struct queue_message {
  char data[4096];
  size_t size;
};

int main(int argc, char **argv) {
  sd_notify(0, "STATUS=Starting");
  logging::Logger logger{"main"};

  boost::lockfree::spsc_queue<queue_message> queue(128);
  std::condition_variable condition_variable;
  std::mutex mutex;
  std::unique_lock<std::mutex> lock(mutex);

  logger.log_info("Building filter app");
  pwcpp::filter::AppBuilder<std::nullptr_t> builder;
  builder.set_filter_name("pmx-osc-network-sender").set_media_type("Osc").
          set_media_class("Osc/Sink").add_arguments(argc, argv).
          add_input_port("pmx-osc", "8 bit raw midi").set_up_parameters().
          add("target.ip_address", "127.0.0.1").add("target.port", 3300).
          add("target.protocol", "udp").finish().add_signal_processor(
            [&queue, &condition_variable](auto position, auto &in_ports,
                                          auto &out_ports, auto &user_data,
                                          auto &parameters) {
              auto buffer = in_ports[0]->get_buffer();

              if (buffer.has_value()) {
                auto pod = buffer.value().get_pod(0);
                if (pod.has_value()) {
                  if (spa_pod_is_sequence(pod.value())) {
                    const auto sequence = reinterpret_cast<struct
                      spa_pod_sequence*>(pod.value());

                    spa_pod_control *pod_control;
                    SPA_POD_SEQUENCE_FOREACH(sequence, pod_control) {
                      if (pod_control->type != SPA_CONTROL_OSC) {
                        continue;
                      }

                      uint8_t *data = nullptr;
                      uint32_t length;
                      spa_pod_get_bytes(&pod_control->value,
                                        (const void**)&data, &length);

                      queue_message message{};
                      std::copy_n(data, length, message.data);
                      message.size = length;

                      queue.push(message);
                      condition_variable.notify_all();
                    }
                  }
                }

                buffer.value().finish();
              }
            });

  auto app = builder.build();

  std::atomic<bool> running(true);
  logger.log_info("Starting network sender");
  std::thread network_sender(
    [&running, &queue, &lock, &condition_variable, &app]() {
      logging::Logger logger{"network_sender_thread"};

      const auto ip_address_it = std::find_if(
        app.value()->parameters_property->parameters().begin(),
        app.value()->parameters_property->parameters().end(),
        [](const auto &parameter) {
          return get<0>(parameter) == "target.ip_address";
        });

      if (ip_address_it == app.value()->parameters_property->parameters().
                               end()) {
        logger.log_error("Couldn't find ip address parameter");
        return;
      }

      const auto port_it = std::find_if(
        app.value()->parameters_property->parameters().begin(),
        app.value()->parameters_property->parameters().end(),
        [](const auto &parameter) {
          return get<0>(parameter) == "target.port";
        });

      if (port_it == app.value()->parameters_property->parameters().end()) {
        logger.log_error("Couldn't find port parameter");
        return;
      }

      const auto protocol_it = std::find_if(
        app.value()->parameters_property->parameters().begin(),
        app.value()->parameters_property->parameters().end(),
        [](const auto &parameter) {
          return get<0>(parameter) == "target.protocol";
        });

      if (protocol_it == app.value()->parameters_property->parameters().end()) {
        logger.log_error("Couldn't find protocol parameter");
        return;
      }

      boost::asio::io_context io_context;
      boost::asio::ip::udp::resolver resolver(io_context);
      const boost::asio::ip::udp::resolver::results_type endpoints = resolver.
        resolve(boost::asio::ip::udp::v4(),
                std::get<std::string>(std::get<1>(*ip_address_it)),
                std::to_string(std::get<int>(std::get<1>(*port_it))));
      boost::asio::ip::udp::socket socket(io_context);
      socket.open(boost::asio::ip::udp::v4());

      while (running) {
        logger.log_info("Starting queue processing");
        queue_message message{};
        if (queue.pop(message)) {
          if (std::get<std::string>(std::get<1>(*protocol_it)) == "udp") {
            socket.send_to(boost::asio::buffer(message.data, message.size),
                           *endpoints.begin());
          } else {
            logger.log_error("Unknown protocol");
          }
        } else {
          condition_variable.wait(lock);
        }
      }
    });

  if (app.has_value()) {
    logger.log_info("Starting filter app");
    sd_notify(0, "READY=1");
    app.value()->run();
  }

  logger.log_info("Shutting down");
  running = false;
  condition_variable.notify_all();
  network_sender.join();

  return 0;
}
