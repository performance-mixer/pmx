#include "console/commands.h"

#include "wpcpp/proxy_collection_builder.h"
#include "wpcpp/link_collection.h"

#include <iostream>
#include <replxx.hxx>
#include <sstream>
#include <pwcpp/filter/app_builder.h>
#include <condition_variable>
#include <logging/logger.h>
#include <proxy/proxy_watcher.h>
#include <wpcpp/metadata.h>
#include <systemd/sd-bus.h>

#include <sdcpp/bus.h>
#include <sdcpp/error.h>

#include <CLI/CLI11.hpp>

int main(const int argc, char *argv[]) {
  replxx::Replxx repl;
  std::string prompt("pmx ~> ");

  console::request_message request{};
  console::response_message response;

  std::atomic has_request(false);
  std::atomic has_response(false);

  pwcpp::filter::AppBuilder<std::nullptr_t> builder;
  builder.set_filter_name("pmx-console").set_media_type("OSC").
          set_media_class("OSC/Source").add_arguments(argc, argv).
          add_output_port("pmx-osc", "8 bit raw midi").add_signal_processor(
            [&has_request, &response, &has_response, &request](
            auto position, auto &in_ports, auto &out_ports, auto &user_data,
            auto &parameters) {
              if (!has_request) { return; }

              auto out_buffer = out_ports[0]->get_buffer();
              if (out_buffer.has_value()) {
                if (has_request) {
                  switch (request.command) {
                  case console::command::send:
                    {
                      auto spa_data = out_buffer->get_spa_data(0);
                      spa_pod_builder pod_builder{};
                      spa_pod_frame frame{};
                      spa_pod_builder_init(&pod_builder, spa_data->data,
                                           spa_data->maxsize);
                      spa_pod_builder_push_sequence(&pod_builder, &frame, 0);

                      spa_pod_builder_control(&pod_builder, 0, SPA_CONTROL_OSC);
                      spa_pod_builder_bytes(&pod_builder, request.data,
                                            request.size);

                      [[maybe_unused]] auto pod = static_cast<spa_pod*>(
                        spa_pod_builder_pop(&pod_builder, &frame));

                      spa_data->chunk->size = pod_builder.state.offset;
                      break;
                    }
                  }

                  has_request = false;
                  response.message = "OK";
                  has_response = true;
                  has_response.notify_all();
                } else {
                  auto spa_data = out_buffer.value().buffer->buffer->datas[0];
                  spa_data.chunk->offset = 0;
                  spa_data.chunk->size = 0;
                  spa_data.chunk->stride = 1;
                  spa_data.chunk->flags = 0;
                }
                out_buffer->finish();
              } else if (has_request) {
                response.message = "No buffer";
                response.status = console::response_message_status::error;
                has_request = false;
                has_response = true;
                has_response.notify_all();
                out_buffer->finish();
              }
            });

  proxy::ProxyWatcher proxy_watcher;
  auto filter_app = builder.build();

  std::thread pw_thread([&filter_app, &proxy_watcher]() {
    if (filter_app.has_value()) {
      const auto context = pw_context_new(
        pw_main_loop_get_loop(filter_app.value()->loop), nullptr, 0);
      const auto core = pw_context_connect(context, nullptr, 0);
      const auto registry = pw_core_get_registry(core, PW_VERSION_REGISTRY, 0);
      proxy_watcher.register_callback(registry);
      filter_app.value()->run();
    }
  });

  struct WirePlumberControl {
    GOptionContext *context = nullptr;
    GMainLoop *loop = nullptr;
    WpCore *core = nullptr;
    WpObjectManager *object_manager = nullptr;
  };

  std::atomic initialized = false;
  std::shared_ptr<wpcpp::ProxyCollection> proxy_collection;
  std::shared_ptr<wpcpp::LinkCollection> link_collection;
  wpcpp::Metadata metadata;

  std::thread wp_thread(
    [&proxy_collection, &initialized, &metadata, &link_collection]() {
      logging::Logger logger{"main"};
      logger.log_info("Initializing wire plumber");
      WirePlumberControl wire_plumber_control;
      wp_init(WP_INIT_ALL);

      wire_plumber_control.context = g_option_context_new("pmx-grpc-api");
      wire_plumber_control.loop = g_main_loop_new(nullptr, false);
      wire_plumber_control.core = wp_core_new(nullptr, nullptr, nullptr);
      if (wp_core_connect(wire_plumber_control.core)) {
        logger.log_info("Creating proxy collection");
        wpcpp::ProxyCollectionBuilder builder;
        builder.add_interest(WP_TYPE_NODE).add_interest(WP_TYPE_PORT);
        proxy_collection = builder.build(wire_plumber_control.core);
        link_collection = wpcpp::LinkCollection::create_and_setup(
          wire_plumber_control.core);
        metadata.get_existing_metadata_object(wire_plumber_control.core);
        initialized = true;
        initialized.notify_all();
        g_main_loop_run(wire_plumber_control.loop);
      }
    });

  initialized.wait(false);
  repl.install_window_change_handler();

  sd_bus *sd_bus = nullptr;
  auto sd_result = sd_bus_open_user(&sd_bus);
  if (sd_result < 0) {
    std::cout << "could not open dbus system bus: " << strerror(-sd_result) <<
      std::endl;
    return -1;
  }

  sdcpp::Bus bus{sd_bus};
  CLI::App app{"pmx-console"};

  auto exit_command = app.add_subcommand("exit", "Exit the console");
  auto start_command = app.add_subcommand("start",
                                          "Start pmx-1 services and filters");
  auto status_command = app.add_subcommand("status",
                                           "Show status of pmx-1 service, filters and connections");

  while (true) {
    char const *line = repl.input(prompt);
    if (line == nullptr) {
      break;
    }

    try {
      std::vector<std::string> args = CLI::detail::split_up(line);
      app.parse(args);
      std::expected<void, sdcpp::error> result = {};
      if (exit_command->parsed()) {
        break;
      } else if (start_command->parsed()) {
        result = console::start_command(bus);
      } else if (status_command->parsed()) {
        result = console::status_command(*proxy_collection, *link_collection,
                                         bus);
      }

      if (!result) {
        std::cout << "There was an error: " << result.error().message <<
          std::endl;
      }

      continue;
    } catch (CLI::ParseError &e) {
      std::cout << "Error parsing command: " << e.what() << std::endl;
      std::cout << app.help() << std::endl;
    }

    std::string_view command(line);
    if (command == "exit") {
      break;
    } else if (command == "help") {
      std::cout.flush();
      std::cout.flush();
    } else {
      std::istringstream iss(line);
      std::string token;
      if (iss >> token) {
        if (token == "send") {
          repl.history_add(line);
          auto result = console::send_command(iss, request);
          if (result) {
            has_request = true;
            has_response.wait(false);
            has_response = false;
            std::cout << response.message << std::endl;
          } else {
            std::cout << "There was an error: " << result.error().message <<
              std::endl;
          }
        } else if (token == "watch") {
          repl.history_add(line);
          auto result = console::watch_command(iss, proxy_watcher);
          if (!result) {
            std::cout << "There was an error: " << result.error().message <<
              std::endl;
          }
        } else if (token == "list") {
          repl.history_add(line);
          auto result = console::list_command(iss, *link_collection,
                                              *proxy_collection, bus);
          if (!result) {
            std::cout << "There was an error: " << result.error().message <<
              std::endl;
          }
        } else if (token == "describe") {
          repl.history_add(line);
          auto result = console::describe_command(
            iss, *proxy_collection, proxy_watcher, *link_collection);
        } else if (token == "meta") {
          repl.history_add(line);
          auto result = console::meta_command(iss, metadata);
          if (!result) {
            std::cout << "There was an error: " << result.error().message <<
              std::endl;
          }
        } else if (token == "status") {
          repl.history_add(line);
          auto result = console::status_command(*proxy_collection,
                                                *link_collection, bus);
          if (!result) {
            std::cout << "There was an error: " << result.error().message <<
              std::endl;
          }
        } else if (token == "pmx") {
          repl.history_add(line);
          auto result = console::pmx_command(iss, metadata, *proxy_collection,
                                             *link_collection, sd_bus);
          if (!result) {
            std::cout << "There was an error: " << result.error().message <<
              std::endl;
          }
        } else if (token == "setup") {
          repl.history_add(line);
          auto result = console::setup_command(iss, *proxy_collection,
                                               proxy_watcher,
                                               filter_app.value()->loop);
          if (!result) {
            std::cout << "There was an error: " << result.error().message <<
              std::endl;
          }
        } else {
          std::cout << "Unknown command: " << token << std::endl;
        }
      }
    }
  }

  sd_bus_close(sd_bus);
  sd_bus_unref(sd_bus);
  std::cout << "Goodbye!" << std::endl;
}
