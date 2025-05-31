#include <iostream>
#include <oscpp/server.hpp>
#include <set>
#include <oscpp/client.hpp>

#include "logging/logger.h"

#include <systemd/sd-daemon.h>

#include <pwcpp/filter/app_builder.h>
#include <pwcpp/osc/parse_osc.h>

#include "ump/create_ump_message.h"
#include "slp_ctrl/GroupChannelSooperLoopers.h"

enum class State {
  Off,
  WaitStart,
  Recording,
  WaitStop,
  Playing,
  Overdubbing,
  Multiplying,
  Inserting,
  Replacing,
  Delay,
  Muted,
  Scratching,
  OneShot,
  Substitute,
  Paused,
  Unknown,
};

State translate_state(const float state) {
  if (state == 0.0f) {
    return State::Off;
  }
  if (state == 1.0f) {
    return State::WaitStart;
  }
  if (state == 2.0f) {
    return State::Recording;
  }
  if (state == 3.0f) {
    return State::WaitStop;
  }
  if (state == 4.0f) {
    return State::Playing;
  }
  if (state == 5.0f) {
    return State::Overdubbing;
  }
  if (state == 6.0f) {
    return State::Multiplying;
  }
  if (state == 7.0f) {
    return State::Inserting;
  }
  if (state == 8.0f) {
    return State::Replacing;
  }
  if (state == 9.0f) {
    return State::Delay;
  }
  if (state == 10.0f) {
    return State::Muted;
  }
  if (state == 11.0f) {
    return State::Scratching;
  }
  if (state == 12.0f) {
    return State::OneShot;
  }
  if (state == 13.0f) {
    return State::Substitute;
  }
  if (state == 14.0f) {
    return State::Paused;
  }

  return State::Unknown;
}

struct StateUpdate {
  uint32_t loop_index;
  State state;
};

struct SooperLooperState {
  State state;
  size_t loop_index;
};

struct SooperLoopers {
  std::array<SooperLooperState, 8> group_channel_loopers = {
    SooperLooperState{.state = State::Off, .loop_index = 0},
    SooperLooperState{.state = State::Off, .loop_index = 1},
    SooperLooperState{.state = State::Off, .loop_index = 2},
    SooperLooperState{.state = State::Off, .loop_index = 3},
    SooperLooperState{.state = State::Off, .loop_index = 4},
    SooperLooperState{.state = State::Off, .loop_index = 5},
    SooperLooperState{.state = State::Off, .loop_index = 6},
    SooperLooperState{.state = State::Off, .loop_index = 7}
  };
};

int main(const int argc, char *argv[]) {
  sd_notify(0, "STATUS=Starting");
  logging::Logger logger{"main"};

  slp::ctrl::GroupChannelSooperLoopers loopers;

  uint64_t ppm_count{0};
  auto sooper_loopers = SooperLoopers{};

  float known_eighth_per_cycle{64.0f};

  pwcpp::filter::AppBuilder<std::nullptr_t> builder;
  builder.set_filter_name("pmx-slp-ctrl").set_media_type("application/control").
          set_media_class("application/control").add_arguments(argc, argv).
          add_input_port("pmx-from-sl-osc", "8 bit raw control").
          add_input_port("lp_mini", "32 bit raw UMP").
          add_input_port("midi_clock", "32 bit raw UMP").
          add_output_port("pmx-to-sl-osc", "8 bit raw control").
          add_output_port("lp_mini", "32 bit raw UMP").add_signal_processor(
            [&ppm_count, &sooper_loopers, &known_eighth_per_cycle](
            auto position, auto &in_ports, auto &out_ports, auto &user_data,
            auto &parameters) {
              logging::Logger logger{"signal-processor"};

              float eighth_per_cycle{64};

              auto sl_osc_input_buffer = in_ports[0]->get_buffer();
              if (sl_osc_input_buffer.has_value()) {
                auto packets = pwcpp::osc::parse_osc<16>(
                  sl_osc_input_buffer.value());
                if (packets.has_value()) {
                  std::vector<StateUpdate> state_updates;
                  for (auto &&packet : packets.value()) {
                    if (packet.has_value()) {
                      OSCPP::Server::Message message(packet.value());
                      OSCPP::Server::ArgStream arguments_stream(message.args());

                      std::string address(message.address());

                      if (address == "/sl/global") {
                        auto huh = arguments_stream.int32();
                        auto parameter_name = std::string(
                          arguments_stream.string());
                        auto value = arguments_stream.float32();
                        if (parameter_name == "eighth_per_cycle") {
                          if (value != eighth_per_cycle) {
                            logger.log_warning(
                              "eight per cycle count changed to " +
                              std::to_string(value));
                          }
                          known_eighth_per_cycle = value;
                        }
                      } else if (address == "/sl/loops") {
                        auto loop_index = arguments_stream.int32();
                        auto parameter_name = std::string(
                          arguments_stream.string());
                        auto value = arguments_stream.float32();
                        std::cout << "Received OSC message: " << value <<
                          std::endl;
                        if (parameter_name == "state") {
                          state_updates.push_back({
                            loop_index, translate_state(value)
                          });
                        }
                      }
                    }
                  }

                  for (auto state_update : state_updates) {
                    if (state_update.loop_index < 8) {
                      sooper_loopers.group_channel_loopers[state_update.loop_index].
                        state = state_update.state;
                    }
                  }
                }

                sl_osc_input_buffer.value().finish();
              }

              std::set<int> registered_loop_ctrl_presses;
              bool switch_to_mixer_mode{false};
              auto lp_mini_input_buffer = in_ports[1]->get_buffer();
              if (lp_mini_input_buffer.has_value()) {
                auto pod = lp_mini_input_buffer.value().get_pod(0);
                auto sequence = reinterpret_cast<struct spa_pod_sequence*>(pod.
                  value());

                spa_pod_control *pod_control;
                SPA_POD_SEQUENCE_FOREACH(sequence, pod_control) {
                  if (pod_control->type != SPA_CONTROL_UMP) {
                    continue;
                  }

                  const void *data = SPA_POD_BODY(&pod_control->value);
                  uint32_t length = SPA_POD_BODY_SIZE(&pod_control->value);

                  if (length == 4) {
                    auto message = static_cast<const uint32_t*>(data);
                    std::cout << "Received UMP message: " << message[0] <<
                      std::endl;
                  } else if (length == 8) {
                    auto message = static_cast<const uint32_t*>(data);
                    if (message[0] == 1082151168 && message[1] == 0) {
                      registered_loop_ctrl_presses.insert(0);
                    } else if (message[0] == 1082151424 && message[1] == 0) {
                      registered_loop_ctrl_presses.insert(1);
                    } else if (message[0] == 1082151680 && message[1] == 0) {
                      registered_loop_ctrl_presses.insert(2);
                    } else if (message[0] == 1082151936 && message[1] == 0) {
                      registered_loop_ctrl_presses.insert(3);
                    } else if (message[0] == 1082152192 && message[1] == 0) {
                      registered_loop_ctrl_presses.insert(4);
                    } else if (message[0] == 1082152448 && message[1] == 0) {
                      registered_loop_ctrl_presses.insert(5);
                    } else if (message[0] == 1082152704 && message[1] == 0) {
                      registered_loop_ctrl_presses.insert(6);
                    } else if (message[0] == 1082152960 && message[1] == 0) {
                      registered_loop_ctrl_presses.insert(7);
                    } else if (message[0] == 1085298944 && message[1] == 0) {
                      switch_to_mixer_mode = true;
                      std::cout << "Switching to mixer mode -> control received"
                        << std::endl;
                    } else if (message[0] == 1085538560) {
                      std::cout << "Fader 1 value: " << message[1] << std::endl;
                    } else if (message[0] == 1085538816) {
                      std::cout << "Fader 2 value: " << message[1] << std::endl;
                    } else if (message[0] == 1085539072) {
                      std::cout << "Fader 3 value: " << message[1] << std::endl;
                    } else if (message[0] == 1085539328) {
                      std::cout << "Fader 4 value: " << message[1] << std::endl;
                    } else if (message[0] == 1085539584) {
                      std::cout << "Fader 5 value: " << message[1] << std::endl;
                    } else if (message[0] == 1085540608) {
                      std::cout << "Fader 6 value: " << message[1] << std::endl;
                    } else if (message[0] == 1085540096) {
                      std::cout << "Fader 7 value: " << message[1] << std::endl;
                    } else if (message[0] == 1085540352) {
                      std::cout << "Fader 8 value: " << message[1] << std::endl;
                    } else {
                      std::cout << "Unknown UMP message: [0]=" << message[0] <<
                        " [1]=" << message[1] << std::endl;
                    }
                  }
                }

                lp_mini_input_buffer.value().finish();

                if (!registered_loop_ctrl_presses.empty()) {
                  std::cout << "Registered loop control presses: ";
                  for (const auto &press : registered_loop_ctrl_presses) {
                    std::cout << press << " ";
                  }
                  std::cout << std::endl;
                }
              }

              auto midi_clock_input_buffer = in_ports[2]->get_buffer();
              if (midi_clock_input_buffer.has_value()) {
                auto pod = midi_clock_input_buffer.value().get_pod(0);
                auto sequence = reinterpret_cast<struct spa_pod_sequence*>(pod.
                  value());

                spa_pod_control *pod_control;
                SPA_POD_SEQUENCE_FOREACH(sequence, pod_control) {
                  if (pod_control->type != SPA_CONTROL_UMP) {
                    continue;
                  }

                  const void *data = SPA_POD_BODY(&pod_control->value);
                  uint32_t length = SPA_POD_BODY_SIZE(&pod_control->value);

                  if (length == 4) {
                    auto message = static_cast<const uint32_t*>(data);
                    if (message[0] == 284884992) {
                      ppm_count = 0;
                    } else if (message[0] == 284688384) {
                      ppm_count++;
                    }
                  }
                }
                midi_clock_input_buffer.value().finish();
              }

              auto sl_osc_out_buffer = out_ports[0]->get_buffer();
              if (sl_osc_out_buffer.has_value()) {
                auto spa_data = sl_osc_out_buffer->get_spa_data(0);
                spa_pod_builder builder{};
                spa_pod_frame frame{};
                spa_pod_builder_init(&builder, spa_data->data,
                                     spa_data->maxsize);
                spa_pod_builder_push_sequence(&builder, &frame, 0);

                for (const auto &loop_index : registered_loop_ctrl_presses) {
                  std::string path = "/sl/" + std::to_string(loop_index) +
                    "/hit";
                  std::string arg = "record";

                  char osc_buffer[4096];
                  OSCPP::Client::Packet packet(osc_buffer, 4096);
                  packet.openMessage(path.c_str(), 1).string(arg.c_str()).
                         closeMessage();

                  spa_pod_builder_control(&builder, 0, SPA_CONTROL_OSC);
                  spa_pod_builder_bytes(&builder, packet.data(), packet.size());
                }

                if (known_eighth_per_cycle != eighth_per_cycle) {
                  logger.log_info("Sending eighth per cycle update");
                  char osc_buffer[4096];
                  OSCPP::Client::Packet packet(osc_buffer, 4096);
                  packet.openMessage("/set", 2).string("eighth_per_cycle").
                         float32(64.0f).closeMessage();
                  spa_pod_builder_control(&builder, 0, SPA_CONTROL_OSC);
                  spa_pod_builder_bytes(&builder, packet.data(), packet.size());
                }

                [[maybe_unused]] auto pod = static_cast<spa_pod*>(
                  spa_pod_builder_pop(&builder, &frame));

                spa_data->chunk->size = builder.state.offset;
                sl_osc_out_buffer.value().finish();
              }

              auto lp_mini_buffer = out_ports[1]->get_buffer();
              if (lp_mini_buffer.has_value()) {
                auto spa_data = lp_mini_buffer->get_spa_data(0);
                spa_pod_builder builder{};
                spa_pod_frame frame{};
                spa_pod_builder_init(&builder, spa_data->data,
                                     spa_data->maxsize);
                spa_pod_builder_push_sequence(&builder, &frame, 0);

                for (auto &&state_update : sooper_loopers.group_channel_loopers) {
                  if (state_update.state == State::Off) {
                    spa_pod_builder_control(&builder, 0, SPA_CONTROL_UMP);
                    auto ump_message = static_cast<uint32_t>(0b00100000) << 24 |
                      static_cast<uint32_t>(0b10010000) << 16 | 81 +
                      state_update.loop_index << 8 | 45;
                    spa_pod_builder_bytes(&builder, &ump_message, 4);
                  } else if (state_update.state == State::WaitStart) {
                    spa_pod_builder_control(&builder, 0, SPA_CONTROL_UMP);
                    auto ump_message = static_cast<uint32_t>(0b00100000) << 24 |
                      static_cast<uint32_t>(0b10010010) << 16 | 81 +
                      state_update.loop_index << 8 | 9;
                    spa_pod_builder_bytes(&builder, &ump_message, 4);
                  } else if (state_update.state == State::Recording) {
                    spa_pod_builder_control(&builder, 0, SPA_CONTROL_UMP);
                    auto ump_message = static_cast<uint32_t>(0b00100000) << 24 |
                      static_cast<uint32_t>(0b10010000) << 16 | 81 +
                      state_update.loop_index << 8 | 5;
                    spa_pod_builder_bytes(&builder, &ump_message, 4);
                  } else if (state_update.state == State::WaitStop) {
                    spa_pod_builder_control(&builder, 0, SPA_CONTROL_UMP);
                    auto ump_message = static_cast<uint32_t>(0b00100000) << 24 |
                      static_cast<uint32_t>(0b10010010) << 16 | 81 +
                      state_update.loop_index << 8 | 7;
                    spa_pod_builder_bytes(&builder, &ump_message, 4);
                  } else if (state_update.state == State::Playing) {
                    spa_pod_builder_control(&builder, 0, SPA_CONTROL_UMP);
                    auto ump_message = static_cast<uint32_t>(0b00100000) << 24 |
                      static_cast<uint32_t>(0b10010000) << 16 | 81 +
                      state_update.loop_index << 8 | 23;
                    spa_pod_builder_bytes(&builder, &ump_message, 4);
                  }
                }

                std::array<uint32_t, 8> bar_to_cc_mapping = {
                  89, 79, 69, 59, 49, 39, 29, 19
                };
                for (uint32_t i = 0; i < 8; i++) {
                  uint32_t value = 33;
                  if (ppm_count / 24 / 4 % 8 == i) {
                    value = 49;
                  }
                  spa_pod_builder_control(&builder, 0, SPA_CONTROL_UMP);
                  uint32_t ump_message = static_cast<uint32_t>(0b00100000) << 24
                    | static_cast<uint32_t>(0b10110000) << 16 |
                    bar_to_cc_mapping[i] << 8 | value;
                  spa_pod_builder_bytes(&builder, &ump_message, 4);
                }

                if (switch_to_mixer_mode) {
                  std::cout << "Sending mixer setup" << std::endl;
                  spa_pod_builder_control(&builder, 0, SPA_CONTROL_UMP);
                  spa_pod_builder_bytes(&builder,
                                        ump::create_8_byte_message(
                                          ump::message_type::sysex_7bit_start,
                                          {0, 32, 41, 2, 13, 1})->data(), 8);

                  spa_pod_builder_control(&builder, 0, SPA_CONTROL_UMP);
                  spa_pod_builder_bytes(&builder,
                                        ump::create_8_byte_message(
                                          ump::message_type::sysex_7bit_continue,
                                          {0, 1, 0, 1, 1, 42})->data(), 8);

                  spa_pod_builder_control(&builder, 0, SPA_CONTROL_UMP);
                  spa_pod_builder_bytes(&builder,
                                        ump::create_8_byte_message(
                                          ump::message_type::sysex_7bit_continue,
                                          {1, 1, 2, 42, 2})->data(), 8);

                  spa_pod_builder_control(&builder, 0, SPA_CONTROL_UMP);
                  spa_pod_builder_bytes(&builder,
                                        ump::create_8_byte_message(
                                          ump::message_type::sysex_7bit_continue,
                                          {1, 3, 42, 3, 1, 4})->data(), 8);

                  spa_pod_builder_control(&builder, 0, SPA_CONTROL_UMP);
                  spa_pod_builder_bytes(&builder,
                                        ump::create_8_byte_message(
                                          ump::message_type::sysex_7bit_continue,
                                          {42, 4, 1, 5, 49, 5})->data(), 8);

                  spa_pod_builder_control(&builder, 0, SPA_CONTROL_UMP);
                  spa_pod_builder_bytes(&builder,
                                        ump::create_8_byte_message(
                                          ump::message_type::sysex_7bit_continue,
                                          {1, 9, 49, 6, 1, 7})->data(), 8);

                  spa_pod_builder_control(&builder, 0, SPA_CONTROL_UMP);
                  spa_pod_builder_bytes(&builder,
                                        ump::create_8_byte_message(
                                          ump::message_type::sysex_7bit_end,
                                          {49, 7, 1, 8, 49})->data(), 8);

                  std::cout << "Switching to mixer mode" << std::endl;

                  spa_pod_builder_control(&builder, 0, SPA_CONTROL_UMP);
                  spa_pod_builder_bytes(&builder,
                                        ump::create_8_byte_message(
                                          ump::message_type::sysex_7bit_start,
                                          {0, 32, 41, 2, 13, 0})->data(), 8);

                  spa_pod_builder_control(&builder, 0, SPA_CONTROL_UMP);
                  spa_pod_builder_bytes(&builder,
                                        ump::create_8_byte_message(
                                          ump::message_type::sysex_7bit_end,
                                          {13})->data(), 8);
                }

                [[maybe_unused]] auto pod = static_cast<spa_pod*>(
                  spa_pod_builder_pop(&builder, &frame));

                spa_data->chunk->size = builder.state.offset;
                lp_mini_buffer.value().finish();
              }
            });

  auto filter_app = builder.build();
  if (filter_app.has_value()) {
    sd_notify(0, "READY=1");
    logger.log_info("Starting filter app");
    filter_app.value()->run();
  }

  return 0;
}
