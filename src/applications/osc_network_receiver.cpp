#include <pwcpp/filter/app_builder.h>

#include <systemd/sd-daemon.h>

#include <thread>
#include <boost/asio.hpp>
#include <iostream>
#include <boost/lockfree/spsc_queue.hpp>
#include <logging/logger.h>

struct queue_message {
  char data[4096];
  size_t size;
};

int main(int argc, char *argv[]) {
  sd_notify(0, "STATUS=Starting");
  logging::Logger logger{"main"};
  boost::lockfree::spsc_queue<queue_message> queue(128);

  logger.log_info("Building filter app");
  pwcpp::filter::AppBuilder<std::nullptr_t> builder;
  builder.set_filter_name("pmx-osc-network-receiver").set_media_type("Osc").
          set_media_class("Osc/Source").
          add_output_port("pmx-osc", "8 bit raw midi").set_up_parameters().
          add("source.port", std::nullopt).add("source.port", 33334).
          add("source.protocol", "udp").finish().add_signal_processor(
            [&queue](auto position, auto &in_ports, auto &out_ports,
                     auto &user_data, auto &parameters) {
              auto out_buffer = out_ports[0]->get_buffer();
              if (out_buffer.has_value()) {
                if (queue.read_available() > 0) {
                  auto spa_data = out_buffer->get_spa_data(0);
                  spa_pod_builder builder{};
                  spa_pod_frame frame{};
                  spa_pod_builder_init(&builder, spa_data->data,
                                       spa_data->maxsize);
                  spa_pod_builder_push_sequence(&builder, &frame, 0);
                  queue_message message{};
                  while (queue.pop(message)) {
                    spa_pod_builder_control(&builder, 0, SPA_CONTROL_OSC);
                    spa_pod_builder_bytes(&builder, message.data, message.size);
                  }

                  // ReSharper disable once CppDFAUnusedValue
                  [[maybe_unused]] auto pod = static_cast<spa_pod*>(
                    spa_pod_builder_pop(&builder, &frame));

                  spa_data->chunk->size = builder.state.offset;
                }
                out_buffer.value().finish();
              }
            });

  bool is_running(true);
  logger.log_info("Starting network receiver");
  std::thread network_receiver([&is_running, &queue]() {
    boost::asio::io_context io_context;
    boost::asio::ip::udp::socket socket(io_context,
                                        boost::asio::ip::udp::endpoint(
                                          boost::asio::ip::udp::v4(), 33334));

    boost::asio::ip::udp::endpoint sender_endpoint;
    while (is_running) {
      queue_message message{0};
      message.size = socket.receive_from(
        boost::asio::buffer(message.data, 4096), sender_endpoint);
      queue.push(message);
    }
  });

  auto filter_app = builder.build();
  if (filter_app.has_value()) {
    sd_notify(0, "READY=1");
    logger.log_info("Starting filter app");
    filter_app.value()->run();
  }

  is_running = false;
  network_receiver.join();
  return 0;
}
