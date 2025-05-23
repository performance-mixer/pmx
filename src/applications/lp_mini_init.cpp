#include <chrono>
#include <iostream>
#include <optional>
#include <queue>

#include "logging/logger.h"

#include <systemd/sd-daemon.h>

#include <pwcpp/filter/app_builder.h>

enum class TaskType { HandShake, EnableSessionMode, SetupInitialSessionState };

struct Task {
  TaskType type;
  std::optional<std::chrono::time_point<std::chrono::system_clock>>
  last_send_time;
};

int main(const int argc, char **argv) {
  sd_notify(0, "STATUS=Starting");
  logging::Logger logger{"main"};

  logger.log_info("Initializing launchpad mini");

  std::queue<Task> tasks;
  tasks.push(Task{.type = TaskType::HandShake, .last_send_time = std::nullopt});
  tasks.push(Task{
    .type = TaskType::EnableSessionMode, .last_send_time = std::nullopt
  });
  tasks.push(Task{
    .type = TaskType::SetupInitialSessionState, .last_send_time = std::nullopt
  });

  const auto timeout = std::chrono::seconds(5);

  std::queue<uint32_t> handshake_response;
  handshake_response.push(806780416);
  handshake_response.push(807807251);
  handshake_response.push(808649734);

  pwcpp::filter::AppBuilder<std::nullptr_t> builder;
  builder.set_filter_name("pmx-lp-mini-init").
          set_media_type("application/control").
          set_media_class("applcation/control").add_arguments(argc, argv).
          add_input_port("lp_mini_in", "32 bit raw UMP").
          add_output_port("lp_mini_out", "32 bit raw UMP").add_signal_processor(
            [&tasks, &timeout, &handshake_response](
            auto position, auto &in_ports, auto &out_ports, auto &user_data,
            auto &parameters) {
              logging::Logger logger{"signal-processor"};
              auto &active_task = tasks.front();
              // Note: no change needed as front() is valid for both vector and queue
              switch (active_task.type) {
              case TaskType::HandShake:
                {
                  auto in_buffer = in_ports.at(0)->get_buffer();
                  if (in_buffer.has_value()) {
                    auto pod = in_buffer.value().get_pod(0);
                    auto sequence = reinterpret_cast<struct spa_pod_sequence*>(
                      pod.value());

                    spa_pod_control *pod_control;
                    SPA_POD_SEQUENCE_FOREACH(sequence, pod_control) {
                      if (pod_control->type != SPA_CONTROL_UMP) {
                        continue;
                      }

                      const void *data = SPA_POD_BODY(&pod_control->value);
                      uint32_t length = SPA_POD_BODY_SIZE(&pod_control->value);

                      if (length != 8) { continue; }

                      auto message = static_cast<const uint32_t*>(data);

                      if (!handshake_response.empty() && message[0] ==
                        handshake_response.front()) {
                        handshake_response.pop();
                      }

                      if (!handshake_response.empty() && message[1] ==
                        handshake_response.front()) {
                        handshake_response.pop();
                      }
                    }
                    in_buffer.value().finish();
                  }


                  auto out_buffer = out_ports[0]->get_buffer();
                  if (out_buffer.has_value()) {
                    auto spa_data = out_buffer->get_spa_data(0);
                    spa_pod_builder pod_builder{};
                    spa_pod_frame frame{};
                    spa_pod_builder_init(&pod_builder, spa_data->data,
                                         spa_data->maxsize);
                    spa_pod_builder_push_sequence(&pod_builder, &frame, 0);
                    if (!handshake_response.empty() && (!active_task.
                                                         last_send_time.
                                                         has_value() ||
                      active_task.last_send_time.value() + timeout <
                      std::chrono::system_clock::now())) {
                      logger.log_info("Sending device id request");
                      spa_pod_builder_control(&pod_builder, 0, SPA_CONTROL_UMP);

                      uint32_t ump_message[2] = {
                        (uint32_t)((uint32_t)0b00110000 << 24 | (uint32_t)4 <<
                          16 | (uint32_t)126 << 8 | 127),
                        (uint32_t)((uint32_t)6 << 24 | (uint32_t)1 << 16 | 0 <<
                          8 | 0)
                      };
                      spa_pod_builder_bytes(&pod_builder, ump_message, 8);
                      active_task.last_send_time =
                        std::chrono::system_clock::now();
                    }

                    [[maybe_unused]] auto pod = static_cast<spa_pod*>(
                      spa_pod_builder_pop(&pod_builder, &frame));

                    spa_data->chunk->size = pod_builder.state.offset;

                    out_buffer->finish();
                  }

                  if (handshake_response.empty()) {
                    logger.log_info("Device id received, handshake complete");
                    tasks.pop();
                  }

                  break;
                }
              case TaskType::EnableSessionMode:
                {
                  auto in_buffer = in_ports.at(0)->get_buffer();
                  if (in_buffer.has_value()) {
                    in_buffer.value().finish();
                  }

                  auto out_buffer = out_ports[0]->get_buffer();
                  if (out_buffer.has_value()) {
                    auto spa_data = out_buffer->get_spa_data(0);
                    spa_pod_builder pod_builder{};
                    spa_pod_frame frame{};
                    spa_pod_builder_init(&pod_builder, spa_data->data,
                                         spa_data->maxsize);
                    spa_pod_builder_push_sequence(&pod_builder, &frame, 0);

                    if (!active_task.last_send_time.has_value()) {
                      logger.log_info("Activating session mode");

                      uint32_t ump_message[4] = {
                        (uint32_t)((uint32_t)0b00110000 << 24 | (uint32_t)
                          0b00010110 << 16 | (uint32_t)0 << 8 | 32),
                        (uint32_t)((uint32_t)41 << 24 | (uint32_t)2 << 16 | (
                          uint32_t)13 << 8 | 16),
                        (uint32_t)((uint32_t)0b00110000 << 24 | (uint32_t)
                          0b00110001 << 16 | (uint32_t)1 << 8 | 0),
                        (uint32_t)((uint32_t)0 << 24 | (uint32_t)0 << 16 | (
                          uint32_t)0 << 8 | 0)
                      };

                      spa_pod_builder_control(&pod_builder, 0, SPA_CONTROL_UMP);
                      spa_pod_builder_bytes(&pod_builder, ump_message, 16);

                      spa_pod_builder_control(&pod_builder, 0, SPA_CONTROL_UMP);
                      spa_pod_builder_bytes(&pod_builder, &ump_message[2], 16);

                      active_task.last_send_time =
                        std::chrono::system_clock::now();
                    }

                    [[maybe_unused]] auto pod = static_cast<spa_pod*>(
                      spa_pod_builder_pop(&pod_builder, &frame));

                    spa_data->chunk->size = pod_builder.state.offset;

                    out_buffer.value().finish();

                    tasks.pop();
                  }

                  break;
                }
                case TaskType::SetupInitialSessionState:
                {
                  auto in_buffer = in_ports.at(0)->get_buffer();
                  if (in_buffer.has_value()) {
                    in_buffer.value().finish();
                  }

                  auto out_buffer = out_ports[0]->get_buffer();
                  if (out_buffer.has_value()) {
                    auto spa_data = out_buffer->get_spa_data(0);
                    spa_pod_builder pod_builder{};
                    spa_pod_frame frame{};
                    spa_pod_builder_init(&pod_builder, spa_data->data,
                                         spa_data->maxsize);
                    spa_pod_builder_push_sequence(&pod_builder, &frame, 0);

                    if (!active_task.last_send_time.has_value()) {
                      logger.log_info("Setting up initial session state");
                      spa_pod_builder_control(&pod_builder, 0, SPA_CONTROL_UMP);
                      uint32_t ump_message = (uint32_t)((uint32_t)0b00100000 <<
                        24 | (uint32_t)0b10000001 << 16 | (uint32_t)81 << 8 |
                        144);
                      spa_pod_builder_bytes(&pod_builder, &ump_message, 4);
                      active_task.last_send_time =
                        std::chrono::system_clock::now();
                    }

                    [[maybe_unused]] auto pod = static_cast<spa_pod*>(
                      spa_pod_builder_pop(&pod_builder, &frame));

                    spa_data->chunk->size = pod_builder.state.offset;

                    out_buffer.value().finish();
                  }
                }
              }
            });


  auto filter_app = builder.build();
  if (filter_app.has_value()) {
    logger.log_info("Starting filter app");
    sd_notify(0, "READY=1");
    filter_app.value()->run();
  }

  sd_notify(0, "READY=1\nSTATUS=Launchpad Mini Initialized");
}
