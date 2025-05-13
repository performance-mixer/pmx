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

inline std::string process_group_channels_port(logging::Logger &logger,
                                               pwcpp::Buffer &in_buffer,
                                               spa_pod_builder &
                                               sequence_builder,
                                               const std::string &
                                               active_layer) {
  std::array<std::optional<pwcpp::midi::message>, 16> midi_messages{};
  get_midi_messages(logger, midi_messages, in_buffer);

  auto count = std::count_if(midi_messages.begin(), midi_messages.end(),
                             [](const auto &midi_message) {
                               return midi_message.has_value();
                             });

  if (count > 0) {
    auto current_layer = active_layer;
    for (auto &&midi_message : midi_messages) {
      if (midi_message.has_value()) {
        if (holds_alternative<
          pwcpp::midi::control_change>(midi_message.value())) {
          auto control_change = get<pwcpp::midi::control_change>(
            midi_message.value());

          if (control_change.cc_number == 64) {
            constexpr auto mid_point = static_cast<uint32_t>(static_cast<double>
              (std::numeric_limits<uint32_t>::max()) / 2.0);

            if (control_change.value < mid_point) {
              current_layer = "A";
            } else {
              current_layer = "B";
            }
          } else {
            auto parameter =
              parameters::find_target_parameter_for_group_channels(
                control_change.cc_number);

            if (parameter.has_value()) {
              spa_pod_builder_control(&sequence_builder, 0, SPA_CONTROL_OSC);

              char osc_buffer[4096];
              OSCPP::Client::Packet packet(osc_buffer, 4096);

              auto message_path = processing::build_group_channel_osc_path(
                parameter->group_id, *parameter->parameter, current_layer);

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

    return current_layer;
  }

  return active_layer;
}

inline void process_input_channels_port(logging::Logger &logger,
                                        pwcpp::Buffer &in_buffer,
                                        spa_pod_builder &sequence_builder,
                                        const std::string &active_layer,
                                        uint8_t channel_id) {
  std::array<std::optional<pwcpp::midi::message>, 16> midi_messages{};
  get_midi_messages(logger, midi_messages, in_buffer);

  auto count = std::count_if(midi_messages.begin(), midi_messages.end(),
                             [](const auto &midi_message) {
                               return midi_message.has_value();
                             });

  if (count > 0) {
    for (auto &&midi_message : midi_messages) {
      if (midi_message.has_value() && holds_alternative<
        pwcpp::midi::control_change>(midi_message.value())) {
        auto control_change = get<pwcpp::midi::control_change>(
          midi_message.value());

        auto parameter = parameters::find_target_parameter_for_input_channels(
          control_change.cc_number);

        spa_pod_builder_control(&sequence_builder, 0, SPA_CONTROL_OSC);

        char osc_buffer[4096];
        OSCPP::Client::Packet packet(osc_buffer, 4096);

        auto message_path = processing::build_input_channel_osc_path(
          channel_id, *parameter, active_layer);

        packet.openMessage(message_path.c_str(), 1).float32(
          static_cast<float>(interpolation::interpolate(
            *parameter, control_change.value))).closeMessage();
        auto size = packet.size();
        spa_pod_builder_bytes(&sequence_builder, osc_buffer, size);
      }
    }
  }
};

inline std::tuple<uint8_t, uint8_t> process_input_channels_mix_port(logging::Logger &logger,
                                            pwcpp::Buffer &in_buffer,
                                            spa_pod_builder &sequence_builder,
                                            const std::string &active_layer,
                                            uint8_t base_channel_id, uint8_t selected_channel_id) {
  uint8_t new_base_channel_id = base_channel_id;
  uint8_t new_selected_channel_id = selected_channel_id;
  std::array<std::optional<pwcpp::midi::message>, 16> midi_messages{};
  get_midi_messages(logger, midi_messages, in_buffer);

  auto count = std::count_if(midi_messages.begin(), midi_messages.end(),
                             [](const auto &midi_message) {
                               return midi_message.has_value();
                             });

  if (count > 0) {
    for (auto &&midi_message : midi_messages) {
      if (midi_message.has_value()) {
        if (holds_alternative<
          pwcpp::midi::control_change>(midi_message.value())) {
          auto control_change = get<pwcpp::midi::control_change>(
            midi_message.value());

          auto parameter =
            parameters::find_target_parameter_for_input_mix_channels(
              control_change.cc_number);

          spa_pod_builder_control(&sequence_builder, 0, SPA_CONTROL_OSC);

          char osc_buffer[4096];
          OSCPP::Client::Packet packet(osc_buffer, 4096);

          auto message_path = processing::build_input_channel_osc_path(
            new_base_channel_id + std::get<uint8_t>(parameter),
            *std::get<1>(parameter), active_layer);

          packet.openMessage(message_path.c_str(), 1).float32(
            static_cast<float>(interpolation::interpolate(
              *std::get<1>(parameter), control_change.value))).closeMessage();
          auto size = packet.size();
          spa_pod_builder_bytes(&sequence_builder, osc_buffer, size);
        } else if (holds_alternative<pwcpp::midi::note_off>(midi_message.value())) {
          auto note_off = get<pwcpp::midi::note_off>(midi_message.value());
          if (note_off.note == 25) {
            new_base_channel_id = 0;
          } else if (note_off.note == 26) {
            new_base_channel_id = 8;
          } else if (note_off.note == 3) {
            new_selected_channel_id = new_base_channel_id + 1;
          } else if (note_off.note == 6) {
            new_selected_channel_id = new_base_channel_id + 2;
          } else if (note_off.note == 9) {
            new_selected_channel_id = new_base_channel_id + 3;
          } else if (note_off.note == 12) {
            new_selected_channel_id = new_base_channel_id + 4;
          } else if (note_off.note == 15) {
            new_selected_channel_id = new_base_channel_id + 5;
          } else if (note_off.note == 18) {
            new_selected_channel_id = new_base_channel_id + 6;
          } else if (note_off.note == 21) {
            new_selected_channel_id = new_base_channel_id + 7;
          } else if (note_off.note == 24) {
            new_selected_channel_id = new_base_channel_id + 8;
          }
        }
      }
    }
  }
  return std::make_tuple(new_base_channel_id, new_selected_channel_id);
};
}
