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
  builder.set_filter_name("pmx-osc-network-router").set_media_type("Osc").
          set_media_class("Osc/Sink").add_arguments(argc, argv).
          add_input_port("osc", "8 bit raw midi").
          add_parameter("target.ip_address", 1,
                        pwcpp::filter::variant_type("127.0.0.1")).
          add_parameter("target.port", 2, pwcpp::filter::variant_type(3300)).
          add_parameter("target.protocol", 3,
                        pwcpp::filter::variant_type("udp")).
          add_signal_processor(
            [&queue, &condition_variable](auto position, auto &in_ports,
                                          auto &out_ports, auto &user_data,
                                          auto &parameters) {
              auto buffer = in_ports[0]->get_buffer();

              if (buffer.has_value()) {
                auto pod = buffer.value().get_pod(0);
                if (pod.has_value()) {
                  if (spa_pod_is_sequence(pod.value())) {
                    auto sequence = reinterpret_cast<struct spa_pod_sequence*>(
                      pod.value());

                    struct spa_pod_control *pod_control;
                    SPA_POD_SEQUENCE_FOREACH(sequence, pod_control) {
                      if (pod_control->type != SPA_CONTROL_OSC) {
                        continue;
                      }

                      uint8_t *data = nullptr;
                      uint32_t length;
                      spa_pod_get_bytes(&pod_control->value,
                                        (const void**)&data, &length);

                      queue_message message;
                      std::copy(data, data + length, message.data);
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
      auto ip_address = get<std::string>(
        app.value()->parameter_collection.parameters[0].value);
      auto port = get<int>(
        app.value()->parameter_collection.parameters[1].value);
      auto protocol = get<std::string>(
        app.value()->parameter_collection.parameters[2].value);

      boost::asio::io_context io_context;
      boost::asio::ip::udp::resolver resolver(io_context);
      boost::asio::ip::udp::resolver::results_type endpoints = resolver.resolve(
        boost::asio::ip::udp::v4(), ip_address, std::to_string(port));
      boost::asio::ip::udp::socket socket(io_context);
      socket.open(boost::asio::ip::udp::v4());

      while (running) {
        logger.log_info("Starting queue processing");
        queue_message message;
        if (queue.pop(message)) {
          if (protocol == "udp") {
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
