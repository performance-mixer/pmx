#include "oscpp/server.hpp"
#include <cstddef>
#include <iostream>
#include <pwcpp/filter/app_builder.h>
#include <pwcpp/osc/parse_osc.h>

int main(int argc, char *argv[]) {

  pwcpp::filter::AppBuilder<std::nullptr_t> builder;
  builder.set_filter_name("pmx-filter-chain-ctrl")
      .set_media_type("Osc")
      .set_media_class("Osc/Sink")
      .add_arguments(argc, argv)
      .add_input_port("input channels", "8 bit raw midi")
      .add_parameter("input_channel_01.filter_chain_id", 1, std::nullopt)
      .add_parameter("input_channel_02.filter_chain_id", 2, std::nullopt)
      .add_parameter("input_channel_03.filter_chain_id", 3, std::nullopt)
      .add_parameter("input_channel_04.filter_chain_id", 4, std::nullopt)
      .add_parameter("input_channel_05.filter_chain_id", 5, std::nullopt)
      .add_parameter("input_channel_06.filter_chain_id", 6, std::nullopt)
      .add_parameter("input_channel_07.filter_chain_id", 7, std::nullopt)
      .add_parameter("input_channel_08.filter_chain_id", 8, std::nullopt)
      .add_parameter("input_channel_09.filter_chain_id", 9, std::nullopt)
      .add_parameter("input_channel_10.filter_chain_id", 10, std::nullopt)
      .add_parameter("input_channel_11.filter_chain_id", 11, std::nullopt)
      .add_parameter("input_channel_12.filter_chain_id", 12, std::nullopt)
      .add_parameter("input_channel_13.filter_chain_id", 13, std::nullopt)
      .add_parameter("input_channel_14.filter_chain_id", 14, std::nullopt)
      .add_parameter("input_channel_15.filter_chain_id", 15, std::nullopt)
      .add_parameter("input_channel_16.filter_chain_id", 16, std::nullopt)
      .add_signal_processor([](auto position, auto &in_ports, auto &out_ports,
                               auto &user_data, auto &parameters) {
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
