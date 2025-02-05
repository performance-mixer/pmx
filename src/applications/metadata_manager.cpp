#include "config/config.h"
#include "config/prefix.h"
#include "wpcpp/metadata_collection.h"
#include "metadata/metadata_watcher.h"
#include "logging/logger.h"

#include <systemd/sd-daemon.h>

#include <iostream>

#include <wp/wp.h>

struct WirePlumberControl {
  GOptionContext *context = nullptr;
  GMainLoop *loop = nullptr;
  WpCore *core = nullptr;
  WpObjectManager *om = nullptr;
  wpcpp::MetadataCollection metadata;
};

int main(int argc, char **argv) {
  sd_notify(0, "STATUS=Starting");
  logging::Logger logger{"main"};
  auto config = config::read_config("pmx");

  logger.log_info("Initializing wire plumber");
  WirePlumberControl wire_plumber_control;
  wp_init(WP_INIT_ALL);
  wire_plumber_control.context = g_option_context_new("metadata-manager");
  wire_plumber_control.loop = g_main_loop_new(nullptr, false);
  wire_plumber_control.core = wp_core_new(nullptr, nullptr, nullptr);
  if (wp_core_connect(wire_plumber_control.core)) {
    struct callback_data {
      config::config &config;
      wpcpp::MetadataCollection &metadata;
    };

    callback_data callback_data{config, wire_plumber_control.metadata};

    logger.log_info("Creating metadata collection");
    wire_plumber_control.metadata.register_initial_metadata_object(
      wire_plumber_control.core,
      [](GObject *source_object, GAsyncResult *res, gpointer data) {
        auto *callback_data = reinterpret_cast<struct callback_data*>(data);
        for (size_t i = 0; i < callback_data->config.input_channels.size(); i
             ++) {
          if (callback_data->config.input_channels[i].has_value()) {
            auto key = config::Prefix::CHANNEL_PORT_PREFIX + std::to_string(
              i + 1);
            callback_data->metadata.set_metadata_value(
              key, callback_data->config.input_channels[i].value());
          }
        }
      }, &callback_data);

    logger.log_info("Setting up metadata watcher");
    metadata::MetadataWatcher watcher;
    watcher.set_metadata_and_connect_signal(
      wire_plumber_control.metadata.metadata());

    logger.log_info("Starting config writer");
    std::thread config_writer_thread{
      [&watcher, config]() mutable {
        logging::Logger logger{"config_writer_thread"};
        std::vector<metadata::metadata_event> batch;
        while (true) {
          if (watcher.has_metadata_updates()) {
            batch.push_back(watcher.pop_metadata_update());
          } else if (!batch.empty()) {
            bool changed(false);
            for (auto &update_event : batch) {
              if (std::holds_alternative<metadata::metadata_deletion>(
                update_event)) {
                auto deletion = std::get<metadata::metadata_deletion>(
                  update_event);
                if (deletion.key.starts_with(
                  config::Prefix::CHANNEL_PORT_PREFIX)) {
                  auto channel_id = std::stoi(deletion.key.substr(8));
                  config.input_channels[channel_id - 1] = std::nullopt;
                  changed = true;
                }
              } else if (std::holds_alternative<metadata::metadata_update>(
                update_event)) {
                auto update = std::get<metadata::metadata_update>(update_event);
                if (update.key.
                           starts_with(config::Prefix::CHANNEL_PORT_PREFIX)) {
                  auto channel_id = std::stoi(update.key.substr(8));
                  if (config.input_channels[channel_id - 1].has_value()) {
                    if (config.input_channels[channel_id - 1].value() != update.
                      value) {
                      config.input_channels[channel_id - 1] = update.value;
                      changed = true;
                    }
                  } else {
                    config.input_channels[channel_id - 1] = update.value;
                    changed = true;
                  }
                }
              }
            }

            batch.clear();

            if (changed) {
              logger.log_info("Writing config");
              config::write_config("pmx", config);
            }
          } else {
            watcher.wait_for(std::chrono::seconds(1));
          }
        }
      }
    };

    sd_notify(0, "READY=1");

    logger.log_info("Starting main loop");
    g_main_loop_run(wire_plumber_control.loop);

    return 0;
  }
}
