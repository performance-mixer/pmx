#include "osc/parse_osc_path.h"
#include "logging/logger.h"

#include <systemd/sd-daemon.h>

#include <cstddef>
#include <iostream>

#include <oscpp/server.hpp>

#include <pwcpp/filter/app_builder.h>
#include <pwcpp/osc/parse_osc.h>

int main(int argc, char *argv[]) {
  sd_notify(0, "STATUS=Starting");

  pwcpp::filter::AppBuilder<std::nullptr_t> builder;
  builder.set_filter_name("pmx-filter-chain-ctrl").set_media_type("Osc").
          set_media_class("Osc/Sink").add_arguments(argc, argv).
          add_input_port("osc", "8 bit raw midi").
          add_parameter("input_channel_01.filter_chain_id", 1, std::nullopt).
          add_parameter("input_channel_02.filter_chain_id", 2, std::nullopt).
          add_parameter("input_channel_03.filter_chain_id", 3, std::nullopt).
          add_parameter("input_channel_04.filter_chain_id", 4, std::nullopt).
          add_parameter("input_channel_05.filter_chain_id", 5, std::nullopt).
          add_parameter("input_channel_06.filter_chain_id", 6, std::nullopt).
          add_parameter("input_channel_07.filter_chain_id", 7, std::nullopt).
          add_parameter("input_channel_08.filter_chain_id", 8, std::nullopt).
          add_parameter("input_channel_09.filter_chain_id", 9, std::nullopt).
          add_parameter("input_channel_10.filter_chain_id", 10, std::nullopt).
          add_parameter("input_channel_11.filter_chain_id", 11, std::nullopt).
          add_parameter("input_channel_12.filter_chain_id", 12, std::nullopt).
          add_parameter("input_channel_13.filter_chain_id", 13, std::nullopt).
          add_parameter("input_channel_14.filter_chain_id", 14, std::nullopt).
          add_parameter("input_channel_15.filter_chain_id", 15, std::nullopt).
          add_parameter("input_channel_16.filter_chain_id", 16, std::nullopt).
          add_signal_processor(
            [](auto position, auto &in_ports, auto &out_ports, auto &user_data,
               auto &parameters) {
              auto buffer = in_ports[0]->get_buffer();
              if (buffer.has_value()) {
                auto packets = pwcpp::osc::parse_osc<16>(buffer.value());
                if (packets.has_value()) {
                  for (auto &&packet : packets.value()) {
                    if (packet.has_value()) {
                      OSCPP::Server::Message message(packet.value());

                      auto osc_path = osc::parse_osc_path(message.address());
                      bool has_no_value = false;
                      bool value_is_int = false;
                      if (osc_path.channel > parameters.size()) {
                        has_no_value = true;
                      } else {
                        has_no_value = std::holds_alternative<std::nullopt_t>(
                          parameters[osc_path.channel - 1].value);
                        value_is_int = std::holds_alternative<int>(
                          parameters[osc_path.channel - 1].value);
                      }

                      if (!has_no_value && value_is_int) {
                        auto pwcpp_parameter = parameters[osc_path.channel - 1];
                        auto pwcpp_parameter_value = std::get<int>(
                          pwcpp_parameter.value);
                        OSCPP::Server::ArgStream arguments(message.args());
                        auto value = arguments.float32();
                        std::cout << "Received message for device parameter " <<
                          osc_path.parameter->full_name << std::endl <<
                          "on channel " << osc_path.channel << std::endl <<
                          "for node id " << pwcpp_parameter_value << std::endl
                          << "with new value " << value << std::endl <<
                          std::endl;
                      } else {
                        std::cout << "No filter chain id configured for input "
                          "channel, or value is not integer: " << osc_path.
                          channel << std::endl;
                      }
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
    sd_notify(0, "READY=1");
    filter_app.value()->run();
  }
}
