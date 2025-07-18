#include "parameters/parameters.h"
#include "midi/router/midi_router.h"

#include <condition_variable>
#include <systemd/sd-daemon.h>

#include <cstddef>
#include <boost/lockfree/spsc_queue.hpp>
#include <logging/logger.h>

#include <pwcpp/filter/app_builder.h>
#include <pwcpp/midi/control_change.h>

#include <spa/pod/builder.h>
#include <tools/filter_parameter_tools.h>

std::string build_input_channel_osc_path(
  const pwcpp::midi::control_change &message,
  const parameters::parameter &parameter) {
  std::stringstream osc_path;

  osc_path << std::format("/I/A/{}/{}/{}",
                          static_cast<int>(message.channel) + 1,
                          parameter.device_short_name, parameter.name);

  return osc_path.str();
}

struct queue_message {
  int base_channel_id = 0;
  std::string layer;
  int selected_channel_id = 1;
};

int main(const int argc, char *argv[]) {
  sd_notify(0, "STATUS=Starting");
  logging::Logger logger{"main"};

  boost::lockfree::spsc_queue<queue_message, boost::lockfree::capacity<2>> queue
    {};

  std::mutex wait_mutex;
  std::condition_variable wait_condition;

  pwcpp::filter::AppBuilder<std::nullptr_t> builder;
  logger.log_info("Building filter app");
  builder.set_filter_name("pmx-midi-router").
          set_media_type("application/control").
          set_media_class("application/control").add_arguments(argc, argv).
          set_up_parameters().add("activeLayer", "A").add("baseChannelId", 0).
          add("selectedChannelId", 1).finish().
          add_input_port("input channels", "32 bit raw UMP").
          add_input_port("group channels", "32 bit raw UMP").
          add_input_port("input channels mix", "32 bit raw UMP").
          add_output_port("pmx-osc", "8 bit raw control").add_signal_processor(
            [&queue, &wait_condition](auto position, auto &in_ports,
                                      auto &out_ports, auto &parameters,
                                      auto &user_data) {
              logging::Logger logger{"signal_processor"};
              auto out_buffer = out_ports[0]->get_buffer();
              if (out_buffer.has_value() == false) {
                return;
              }

              auto spa_data = out_buffer->get_spa_data(0);
              spa_pod_builder pod_builder{};
              spa_pod_frame frame{};
              spa_pod_builder_init(&pod_builder, spa_data->data,
                                   spa_data->maxsize);
              spa_pod_builder_push_sequence(&pod_builder, &frame, 0);

              auto active_layer_option = tools::get_from_collection<
                std::string>("activeLayer", parameters.parameters());

              std::string active_layer;
              if (active_layer_option.has_value()) {
                active_layer = active_layer_option.value();
              } else {
                active_layer = "A";
              }

              auto base_channel_id_option = tools::get_from_collection<int>(
                "baseChannelId", parameters.parameters());
              int base_channel_id = 0;
              if (base_channel_id_option.has_value()) {
                base_channel_id = base_channel_id_option.value();
              }

              auto selected_channel_id_option = tools::get_from_collection<int>(
                "selectedChannelId", parameters.parameters());
              int selected_channel_id = 1;
              if (selected_channel_id_option.has_value()) {
                selected_channel_id = selected_channel_id_option.value();
              }

              // group channels should be processed before input channels, so
              // that the layer selector is processed before the other updates
              auto group_channels_buffer = in_ports.at(1)->get_buffer();

              bool changes = false;

              if (group_channels_buffer.has_value()) {
                auto new_active_layer =
                  midi::router::process_group_channels_port(
                    logger, group_channels_buffer.value(), pod_builder,
                    active_layer);

                if (new_active_layer != active_layer) {
                  changes = true;
                  active_layer = new_active_layer;
                }
              }

              auto input_channels_mix_buffer = in_ports.at(2)->get_buffer();

              if (input_channels_mix_buffer.has_value()) {
                auto result = midi::router::process_input_channels_mix_port(
                  logger, input_channels_mix_buffer.value(), pod_builder,
                  active_layer, base_channel_id, selected_channel_id);

                if (std::get<0>(result) != base_channel_id) {
                  changes = true;
                  base_channel_id = std::get<0>(result);
                }

                if (std::get<1>(result) != selected_channel_id) {
                  changes = true;
                  selected_channel_id = std::get<1>(result);
                }
              }

              auto input_channels_buffer = in_ports.at(0)->get_buffer();

              if (input_channels_buffer.has_value()) {
                midi::router::process_input_channels_port(
                  logger, input_channels_buffer.value(), pod_builder,
                  active_layer, selected_channel_id);
              }

              if (changes) {
                queue.push(queue_message{
                  .base_channel_id = base_channel_id, .layer = active_layer,
                  .selected_channel_id = selected_channel_id
                });
                wait_condition.notify_all();
              }

              [[maybe_unused]] auto pod = static_cast<spa_pod*>(
                spa_pod_builder_pop(&pod_builder, &frame));

              spa_data->chunk->size = pod_builder.state.offset;

              out_buffer->finish();
            });

  auto filter_app = builder.build();

  bool running(true);
  std::thread update_parameter_value_thread(
    [&queue, &wait_mutex, &wait_condition, &running, &filter_app]() {
      while (running) {
        if (!queue.empty()) {
          std::string layer;
          int base_channel_id = 0;
          int selected_channel_id = 1;
          while (!queue.empty()) {
            queue_message message;
            queue.pop(&message);
            layer = message.layer;
            base_channel_id = message.base_channel_id;
            selected_channel_id = message.selected_channel_id;
          }

          filter_app.value()->parameters_property->update<std::string>(
            "activeLayer", layer);

          filter_app.value()->parameters_property->update<int>(
            "baseChannelId", base_channel_id);

          filter_app.value()->parameters_property->update<int>(
            "selectedChannelId", selected_channel_id);

          std::uint8_t buffer[1024];
          spa_pod_builder builder{};
          spa_pod_builder_init(&builder, buffer, sizeof(buffer));
          spa_pod_frame frame{};
          spa_pod_builder_push_object(&builder, &frame, SPA_TYPE_OBJECT_Props,
                                      SPA_PARAM_Props);
          auto result = filter_app.value()->parameters_property->
                                   add_to_pod_object(&builder);

          if (result.has_value()) {
            const spa_pod *params_pod[1];
            params_pod[0] = static_cast<spa_pod*>(spa_pod_builder_pop(
              &builder, &frame));

            struct invoke_data {
              pw_filter *filter;
              const spa_pod **params;
            };

            invoke_data data{
              .filter = filter_app.value()->filter, .params = params_pod
            };

            pw_loop_invoke(pw_main_loop_get_loop(filter_app.value()->loop),
                           [](spa_loop *loop, bool async, u_int32_t seq,
                              const void *data, size_t size, void *user_data) {
                             auto my_data = static_cast<const invoke_data*>(
                               data);

                             pw_filter_update_params(
                               my_data->filter, nullptr, my_data->params, 1);
                             return 0;
                           }, 1, &data, sizeof(invoke_data), true, nullptr);
          }
        }

        std::unique_lock<std::mutex> lock(wait_mutex);
        wait_condition.wait(lock, [&queue]() { return !queue.empty(); });
      }
    });

  if (filter_app.has_value()) {
    logger.log_info("Starting filter app");
    sd_notify(0, "READY=1");
    filter_app.value()->run();
  }
}
