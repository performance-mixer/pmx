#include "interpolation/interpolation.h"
#include "parameters/parameters.h"

#include <systemd/sd-daemon.h>

#include <cstddef>
#include <iostream>
#include <ranges>
#include <logging/logger.h>

#include <oscpp/client.hpp>

#include <pwcpp/filter/app_builder.h>
#include <pwcpp/midi/control_change.h>
#include <pwcpp/midi/message.h>
#include <pwcpp/midi/parse_midi.h>

#include <spa/pod/builder.h>

std::string build_input_channel_osc_path(const pwcpp::midi::control_change &message,
                                         const parameters::parameter &parameter) {
  std::stringstream osc_path;

  osc_path << std::format("/I/A/{}/{}/{}",
                          static_cast<int>(message.channel) + 1,
                          parameter.device_short_name, parameter.name);

  return osc_path.str();
}

int main(const int argc, char *argv[]) {
  sd_notify(0, "STATUS=Starting");
  logging::Logger logger{"main"};

  pwcpp::filter::AppBuilder<std::nullptr_t> builder;
  logger.log_info("Building filter app");
  builder.set_filter_name("pmx-midi-router").set_media_type("Midi").
          set_media_class("Midi/Sink").add_arguments(argc, argv).
          add_input_port("input channels", "8 bit raw midi").
          add_input_port("output channels", "8 bit raw midi").
          add_output_port("osc", "8 bit raw midi").add_signal_processor(
            [](auto position, auto &in_ports, auto &out_ports, auto &user_data,
               auto &parameters) {
              logging::Logger logger{"signal_processor"};
              auto in_buffers = in_ports | std::views::transform(
                [](auto &&in_port) {
                  return in_port->get_buffer();
                });

              std::array<std::optional<pwcpp::midi::message>, 32> midi_messages
                {};
              auto array_insertion_iter = midi_messages.begin();

              for (auto &&buffer : in_buffers) {
                if (buffer.has_value()) {
                  auto buffer_midi_messages = pwcpp::midi::parse_midi<16>(
                    buffer.value());
                  if (buffer_midi_messages.has_value()) {
                    for (auto midi_message : buffer_midi_messages.value()) {
                      if (midi_message.has_value()) {
                        *array_insertion_iter = midi_message;
                        array_insertion_iter++;
                      }
                    }
                  } else {
                    logger.log_error(buffer_midi_messages.error());
                  }
                  buffer.value().finish();
                }
              }

              int count = std::count_if(midi_messages.begin(),
                                        midi_messages.end(),
                                        [](const auto &midi_message) {
                                          return midi_message.has_value();
                                        });

              if (count > 0) {
                auto out_buffer = out_ports[0]->get_buffer();
                if (out_buffer.has_value() == false) {
                  return;
                }

                auto spa_data = out_buffer->get_spa_data(0);
                spa_pod_builder builder{};
                spa_pod_frame frame{};
                spa_pod_builder_init(&builder, spa_data->data,
                                     spa_data->maxsize);
                spa_pod_builder_push_sequence(&builder, &frame, 0);
                for (auto &&midi_message : midi_messages) {
                  if (midi_message.has_value()) {
                    auto control_change = get<pwcpp::midi::control_change>(
                      midi_message.value());

                    auto parameter = parameters::find_target_parameter(
                      control_change.cc_number);

                    spa_pod_builder_control(&builder, 0, SPA_CONTROL_OSC);

                    char osc_buffer[4096];
                    OSCPP::Client::Packet packet(osc_buffer, 4096);

                    auto message_path = build_input_channel_osc_path(
                      control_change, *parameter);

                    std::cout << message_path << std::endl;

                    packet.openMessage(message_path.c_str(), 1).float32(
                      static_cast<float>(interpolation::interpolate(
                        *parameter, control_change.value))).closeMessage();
                    auto size = packet.size();
                    spa_pod_builder_bytes(&builder, osc_buffer, size);
                  }
                }

                auto pod = static_cast<spa_pod*>(spa_pod_builder_pop(
                  &builder, &frame));

                spa_data->chunk->size = builder.state.offset;
                out_buffer->finish();
              }
            });

  auto filter_app = builder.build();
  if (filter_app.has_value()) {
    logger.log_info("Starting filter app");
    sd_notify(0, "READY=1");
    filter_app.value()->run();
  }
}
