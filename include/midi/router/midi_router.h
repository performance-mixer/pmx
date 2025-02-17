#pragma once

#include <processing.h>
#include <interpolation/interpolation.h>
#include <logging/logger.h>
#include <oscpp/client.hpp>
#include <pwcpp/buffer.h>
#include <pwcpp/midi/parse_midi.h>
#include <spa/pod/builder.h>

namespace midi::router {
inline void get_midi_messages(logging::Logger &logger,
                              std::array<
                                std::optional<pwcpp::midi::message>, 16> &
                              midi_messages, pwcpp::Buffer &in_buffer) {
  auto array_insertion_iter = midi_messages.begin();
  auto buffer_midi_messages = pwcpp::midi::parse_midi<16>(in_buffer);
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
  in_buffer.finish();
}

inline void process_group_channels_port(logging::Logger &logger,
                                        pwcpp::Buffer &in_buffer,
                                        spa_pod_builder &sequence_builder) {
  std::array<std::optional<pwcpp::midi::message>, 16> midi_messages{};
  get_midi_messages(logger, midi_messages, in_buffer);

  auto count = std::count_if(midi_messages.begin(), midi_messages.end(),
                             [](const auto &midi_message) {
                               return midi_message.has_value();
                             });

  if (count > 0) {
    for (auto &&midi_message : midi_messages) {
      if (midi_message.has_value()) {
        auto control_change = get<pwcpp::midi::control_change>(
          midi_message.value());

        auto parameter = parameters::find_target_parameter_for_group_channels(
          control_change.cc_number);

        if (parameter.has_value()) {
          spa_pod_builder_control(&sequence_builder, 0, SPA_CONTROL_OSC);

          char osc_buffer[4096];
          OSCPP::Client::Packet packet(osc_buffer, 4096);

          auto message_path = processing::build_group_channel_osc_path(
            parameter->group_id, *parameter->parameter);

          std::cout << message_path << std::endl;

          packet.openMessage(message_path.c_str(), 1).float32(
            static_cast<float>(interpolation::interpolate(
              *parameter->parameter, control_change.value))).closeMessage();
          auto size = packet.size();
          spa_pod_builder_bytes(&sequence_builder, osc_buffer, size);
        }
      }
    }
  }
}

inline void process_input_channels_port(logging::Logger &logger,
                                        pwcpp::Buffer &in_buffer,
                                        spa_pod_builder &sequence_builder) {
  std::array<std::optional<pwcpp::midi::message>, 16> midi_messages{};
  get_midi_messages(logger, midi_messages, in_buffer);

  auto count = std::count_if(midi_messages.begin(), midi_messages.end(),
                             [](const auto &midi_message) {
                               return midi_message.has_value();
                             });

  if (count > 0) {
    for (auto &&midi_message : midi_messages) {
      if (midi_message.has_value()) {
        auto control_change = get<pwcpp::midi::control_change>(
          midi_message.value());

        auto parameter = parameters::find_target_parameter_for_input_channels(
          control_change.cc_number);

        spa_pod_builder_control(&sequence_builder, 0, SPA_CONTROL_OSC);

        char osc_buffer[4096];
        OSCPP::Client::Packet packet(osc_buffer, 4096);

        auto message_path = processing::build_input_channel_osc_path(
          control_change, *parameter);

        std::cout << message_path << std::endl;

        packet.openMessage(message_path.c_str(), 1).float32(
          static_cast<float>(interpolation::interpolate(
            *parameter, control_change.value))).closeMessage();
        auto size = packet.size();
        spa_pod_builder_bytes(&sequence_builder, osc_buffer, size);
      }
    }
  }
};
}
