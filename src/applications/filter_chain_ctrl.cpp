#include <condition_variable>

#include "filter_chain_control/filter_chain_control.h"
#include "osc/parse_osc_path.h"
#include "logging/logger.h"
#include "tools/pipewire_tools.h"
#include "proxy/proxy_watcher.h"

#include <systemd/sd-daemon.h>

#include <cstddef>
#include <iostream>
#include <thread>
#include <boost/lockfree/spsc_queue.hpp>

#include <oscpp/server.hpp>

#include <pwcpp/filter/app_builder.h>
#include <pwcpp/osc/parse_osc.h>

struct queue_message {
  osc::osc_path path;
  int filter_chain_id = 0;
  double value = 0.0;
};

int main(const int argc, char *argv[]) {
  sd_notify(0, "STATUS=Starting");

  boost::lockfree::spsc_queue<queue_message> queue(128);
  std::mutex wait_mutex;
  std::condition_variable wait_condition;

  pwcpp::filter::AppBuilder<std::nullptr_t> builder;
  builder.set_filter_name("pmx-filter-chain-ctrl").set_media_type("Osc").
          set_media_class("Osc/Sink").add_arguments(argc, argv).
          add_input_port("pmx-osc", "8 bit raw midi").
          add_parameter("input_channels.filter_chain_id", 1, std::nullopt).
          add_parameter("group_channels_a.filter_chain_id", 2, std::nullopt).
          add_parameter("group_channels_a.filter_chain_id", 3, std::nullopt).
          add_parameter("layer_channels.filter_chain_id", 4, std::nullopt).
          add_signal_processor(
            [&queue, &wait_condition](auto position, auto &in_ports,
                                      auto &out_ports, auto &user_data,
                                      auto &parameters) {
              logging::Logger logger{"signal-processor"};

              auto buffer = in_ports[0]->get_buffer();
              if (buffer.has_value()) {
                auto packets = pwcpp::osc::parse_osc<16>(buffer.value());
                if (packets.has_value()) {
                  bool wakeup_consumer(false);
                  for (auto &&packet : packets.value()) {
                    if (packet.has_value()) {
                      OSCPP::Server::Message message(packet.value());

                      auto osc_path = osc::parse_osc_path(message.address());
                      if (osc_path.has_value()) {
                        switch (osc_path->channel_type) {
                        case osc::osc_path_channel_type::input:
                          {
                            if (
                              filter_chain_control::get_filter_chain_id_and_add_to_queue(
                                logger, queue, 1, parameters, *osc_path,
                                message)) {
                              wakeup_consumer = true;
                            }
                            break;
                          }
                        case osc::osc_path_channel_type::group:
                          {
                            std::uint32_t node_parameter_id(0);
                            if (osc_path->layer == osc::osc_path_layer::A) {
                              node_parameter_id = 2;
                            } else {
                              node_parameter_id = 3;
                            }

                            if (
                              filter_chain_control::get_filter_chain_id_and_add_to_queue(
                                logger, queue, node_parameter_id, parameters,
                                *osc_path, message)) {
                              wakeup_consumer = true;
                            }
                            break;
                          }
                        case osc::osc_path_channel_type::layer:
                          {
                            if (
                              filter_chain_control::get_filter_chain_id_and_add_to_queue(
                                logger, queue, 4, parameters, *osc_path,
                                message)) {
                              wakeup_consumer = true;
                            }
                            break;
                          }
                        case osc::osc_path_channel_type::none:
                          break;
                        }
                      } else {
                        logger.log_error(
                          "Invalid OSC path, " + osc_path.error().message,
                          ("OSC_PATH=" + std::string(message.address())).
                          c_str());
                      }
                    }
                  }

                  if (wakeup_consumer) {
                    wait_condition.notify_all();
                  }
                } else {
                  logger.log_error(
                    "Failed to parse OSC packets, " + packets.error().message);
                }

                auto spa_data = buffer.value().buffer->buffer->datas[0];
                spa_data.chunk->offset = 0;
                spa_data.chunk->size = 0;
                spa_data.chunk->stride = 1;
                spa_data.chunk->flags = 0;
                logger.log_info("Finishing buffer");
                buffer.value().finish();
              }
            });

  proxy::ProxyWatcher proxy_watcher;

  auto filter_app = builder.build();

  bool running(false);
  std::thread filter_controller_thread(
    [&queue, &wait_mutex, &wait_condition, &running, &proxy_watcher, &filter_app
    ]() {
      logging::Logger logger{"filter-controller-thread"};
      while (running) {
        while (!queue.empty()) {
          queue_message message{0};
          queue.pop(&message);
          switch (message.path.channel_type) {
          case osc::osc_path_channel_type::input:
            {
              logger.log_info("Processing input channels parameter change");
              std::uint8_t device_number(0);
              if (message.path.layer == osc::osc_path_layer::A) {
                device_number = message.path.channel_id;
              } else {
                device_number = message.path.channel_id * 2;
              }

              auto parameter_name = std::format(
                "{}-{}:{}", message.path.parameter->device_name, device_number,
                message.path.parameter->name);

              auto client = proxy_watcher.get_proxy_client(
                message.filter_chain_id);
              if (client.has_value()) {
                tools::set_props_param(client.value(), filter_app.value()->loop,
                                       parameter_name, message.value);
              } else {
                logger.log_warning(
                  "Couldn't get client for node id " + std::to_string(
                    message.filter_chain_id));
              }
              break;
            }
          case osc::osc_path_channel_type::group:
            {
              logger.log_info("Processing group channels parameter change");
              auto parameter_name = std::format(
                "{}-G{}:{}", message.path.parameter->device_name,
                message.path.channel_id, message.path.parameter->name);

              auto client = proxy_watcher.get_proxy_client(
                message.filter_chain_id);
              if (client.has_value()) {
                tools::set_props_param(client.value(), filter_app.value()->loop,
                                       parameter_name, message.value);
              } else {
                logger.log_warning(
                  "Couldn't get client for node id " + std::to_string(
                    message.filter_chain_id));
              }
              break;
            }
          case osc::osc_path_channel_type::layer:
            {
              std::string parameter_name;
              if (message.path.layer == osc::osc_path_layer::A) {
                parameter_name = std::format("{}-A:{}",
                                             message.path.parameter->
                                                     device_name,
                                             message.path.parameter->name);
              } else {
                parameter_name = std::format("{}-B:{}",
                                             message.path.parameter->
                                                     device_name,
                                             message.path.parameter->name);
              }

              auto client = proxy_watcher.get_proxy_client(
                message.filter_chain_id);
              if (client.has_value()) {
                tools::set_props_param(client.value(), filter_app.value()->loop,
                                       parameter_name, message.value);
              } else {
                logger.log_warning(
                  "Couldn't get client for node id " + std::to_string(
                    message.filter_chain_id));
              }
              break;
            }
          case osc::osc_path_channel_type::none:
            break;
          }
        }

        std::unique_lock<std::mutex> lock(wait_mutex);
        wait_condition.wait(lock, [&queue]() { return !queue.empty(); });
      }
    });

  if (filter_app.has_value()) {
    auto context = pw_context_new(
      pw_main_loop_get_loop(filter_app.value()->loop), nullptr, 0);
    auto core = pw_context_connect(context, nullptr, 0);
    auto registry = pw_core_get_registry(core, PW_VERSION_REGISTRY, 0);
    proxy_watcher.register_callback(registry);
    sd_notify(0, "READY=1");
    filter_app.value()->run(PW_FILTER_FLAG_NONE);
  }
}
