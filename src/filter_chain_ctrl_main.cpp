#include "oscpp/server.hpp"
#include <iostream>
#include <pwcpp/filter/app_builder.h>
#include <pwcpp/osc/parse_osc.h>

int main(int argc, char *argv[]) {

  pwcpp::filter::AppBuilder builder;
  builder.set_filter_name("pmx-filter-chain-ctrl")
      .set_media_type("Osc")
      .set_media_class("Osc/Sink")
      .add_arguments(argc, argv)
      .add_input_port("input channels", "8 bit raw midi")
      .add_signal_processor([](auto position, auto in_ports, auto out_ports) {
        auto buffer = in_ports[0]->get_buffer();
        if (buffer.has_value()) {
          auto packets = pwcpp::osc::parse_osc<16>(buffer.value());
          if (packets.has_value()) {
            for (auto &&packet : packets.value()) {
              if (packet.has_value()) {
                OSCPP::Server::Message message(packet.value());
                std::cout << "Received message: " << message.address()
                          << std::endl;
                OSCPP::Server::ArgStream arguments(message.args());
                auto value = arguments.float32();
                std::cout << "Value: " << value << std::endl;
              }
            }
          } else {
            std::cout << packets.error().message << std::endl;
          }
        }
        buffer.value().finish();
      });

  auto filter_app = builder.build();
  if (filter_app.has_value()) {
    filter_app.value()->run();
  }
}
