#include "config/config.h"
#include "config/prefix.h"
#include "wpcpp/metadata.h"
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
  wpcpp::Metadata metadata;
};

void set_initial_metadata(wpcpp::Metadata &metadata, config::config &config) {
  size_t index = 1;
  for (const auto &channel : config.input_channels) {
    if (channel.has_value()) {
      std::ostringstream metadata_key_ss;
      metadata_key_ss << config::Prefix::CHANNEL_PORT_PREFIX << index;
      metadata.set_metadata_value_async(metadata_key_ss.str(), channel.value());
    }
    ++index;
  }

  index = 1;
  for (const auto &group_id : config.input_channel_groups) {
    std::ostringstream metadata_key_ss;
    metadata_key_ss << config::Prefix::CHANNEL_GROUP_PREFIX << index;
    metadata.set_metadata_value_async(metadata_key_ss.str(),
                                      std::to_string(group_id));
    ++index;
  }
}

int main(int argc, char **argv) {
  sd_notify(0, "STATUS=Starting");
  logging::Logger logger{"main"};
  auto config = config::read_config("pmx");

  logger.log_info("Initializing wire plumber");
  WirePlumberControl wire_plumber_control;
  wp_init(WP_INIT_ALL);
  wire_plumber_control.context = g_option_context_new("metadata-manager");

  auto main_context = g_main_context_new();
  wire_plumber_control.loop = g_main_loop_new(main_context, false);
  wire_plumber_control.core = wp_core_new(main_context, nullptr, nullptr);
  if (wp_core_connect(wire_plumber_control.core)) {
    struct callback_data {
      config::config &config;
      wpcpp::Metadata &metadata;
    };

    logger.log_info("Reading metadata collection");
    wire_plumber_control.metadata.get_existing_metadata_object(
      wire_plumber_control.core);

    /* iterate the main loop until we found the metadata */
    while (wire_plumber_control.metadata.metadata() == nullptr) {
      g_main_context_iteration(main_context, true);
    }

    set_initial_metadata(wire_plumber_control.metadata, config);

    logger.log_info("Setting up metadata watcher");
    metadata::MetadataWatcher watcher;
    logger.log_info(" Setup metadata");
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
                  const auto channel_id = std::stoi(deletion.key.substr(
                    config::Prefix::CHANNEL_PORT_PREFIX.size()));
                  config.input_channels[channel_id - 1] = std::nullopt;
                  changed = true;
                }
              } else if (std::holds_alternative<metadata::metadata_update>(
                update_event)) {
                auto update = std::get<metadata::metadata_update>(update_event);
                if (update.key.
                           starts_with(config::Prefix::CHANNEL_PORT_PREFIX)) {
                  auto channel_id = std::stoi(update.key.substr(
                    config::Prefix::CHANNEL_PORT_PREFIX.size()));
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
                } else if (update.key.starts_with(
                  config::Prefix::CHANNEL_GROUP_PREFIX)) {
                  auto channel_id = std::stoi(update.key.substr(
                    config::Prefix::CHANNEL_GROUP_PREFIX.size()));
                  config.input_channel_groups[channel_id - 1] = std::stoi(
                    update.value);
                  changed = true;
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
