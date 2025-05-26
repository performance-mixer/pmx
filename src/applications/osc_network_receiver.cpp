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

  if (argc < 4) {
    logger.log_error(
      "Usage: " + std::string(argv[0]) +
      " <filter_name> <port_name> <source_port>");
    return 1;
  }

  const std::string filter_name = argv[1];
  const std::string port_name = argv[2];
  const int source_port = std::stoi(argv[3]);

  boost::lockfree::spsc_queue<queue_message> queue(128);

  logger.log_info("Listening on port " + port_name);
  logger.log_info("Building filter app");
  pwcpp::filter::AppBuilder<std::nullptr_t> builder;
  builder.set_filter_name(filter_name).set_media_type("application/control").
          set_media_class("application/control").add_arguments(argc, argv).
          add_output_port(port_name, "8 bit raw control").set_up_parameters().
          add("source.port", source_port).add("source.protocol", "udp").finish()
          .add_signal_processor(
            [&queue](auto position, auto &in_ports, auto &out_ports,
                     auto &user_data, auto &parameters) {
              auto out_buffer = out_ports[0]->get_buffer();
              if (out_buffer.has_value()) {
                logging::Logger logger{"signal-processor"};
                auto spa_data = out_buffer->get_spa_data(0);
                spa_pod_builder builder{};
                spa_pod_frame frame{};
                spa_pod_builder_init(&builder, spa_data->data,
                                     spa_data->maxsize);
                spa_pod_builder_push_sequence(&builder, &frame, 0);
                if (queue.read_available() > 0) {
                  queue_message message{};
                  int count(1);
                  while (queue.pop(message)) {
                    logger.log_info(
                      "Adding OSC message " + std::to_string(count++));
                    spa_pod_builder_control(&builder, 0, SPA_CONTROL_OSC);
                    spa_pod_builder_bytes(&builder, message.data, message.size);
                  }
                }

                [[maybe_unused]] auto pod = static_cast<spa_pod*>(
                  spa_pod_builder_pop(&builder, &frame));

                spa_data->chunk->size = builder.state.offset;
                out_buffer.value().finish();
              }
            });

  bool is_running(true);
  logger.log_info("Starting network receiver");
  std::thread network_receiver([&is_running, &queue, source_port]() {
    boost::asio::io_context io_context;
    boost::asio::ip::udp::socket socket(io_context,
                                        boost::asio::ip::udp::endpoint(
                                          boost::asio::ip::udp::v4(),
                                          source_port));

    boost::asio::ip::udp::endpoint sender_endpoint;
    logging::Logger logger{"network_receiver_thread"};
    logger.log_info("Starting network receiver thread port " + source_port);
    std::cout << "Starting network receiver thread port " << source_port << std::endl;
    while (is_running) {
      queue_message message{0};
      message.size = socket.receive_from(
        boost::asio::buffer(message.data, 4096), sender_endpoint);
      queue.push(message);
      logger.log_info("Received message and added to queue");
      std::cout << "Received message and added to queue" << std::endl;
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
