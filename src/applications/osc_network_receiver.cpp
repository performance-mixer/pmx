#include <pwcpp/filter/app_builder.h>

#include <systemd/sd-daemon.h>

#include <oscpp/server.hpp>

#include <thread>
#include <boost/asio.hpp>
#include <iostream>
#include <array>
#include <boost/lockfree/spsc_queue.hpp>
#include <pwcpp/osc/parse_osc.h>

struct queue_message {
  char data[4096];
  size_t size;
};

int main(int argc, char *argv[]) {
  boost::lockfree::spsc_queue<queue_message> queue(128);

  pwcpp::filter::AppBuilder<std::nullptr_t> builder;
  builder.set_filter_name("pmx-osc-network-receiver").set_media_type("Osc").
          set_media_class("Osc/Source").add_output_port("osc", "8 bit raw midi")
          .add_parameter("source.port", 1, pwcpp::filter::variant_type(33334)).
          add_parameter("source.protocol", 2,
                        pwcpp::filter::variant_type("udp")).
          add_signal_processor([&queue](
            auto position, auto &in_ports, auto &out_ports, auto &user_data,
            auto &parameters) {
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

                  auto pod = static_cast<spa_pod*>(spa_pod_builder_pop(
                    &builder, &frame));

                  spa_data->chunk->size = builder.state.offset;
                }
                out_buffer.value().finish();
              }
            });

  bool is_running(true);
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
    filter_app.value()->run();
  }
  network_receiver.join();
  return 0;
}
