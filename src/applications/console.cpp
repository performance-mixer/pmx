#include "console/messages.h"
#include "console/send_command.h"
#include "console/meta_command.h"
#include "console/pmx_command.h"
#include "console/list_command.h"
#include "console/describe_command.h"
#include "console/status_command.h"
#include "wpcpp/proxy_collection_builder.h"
#include "wpcpp/link_collection.h"

#include <iostream>
#include <replxx.hxx>
#include <sstream>
#include <pwcpp/filter/app_builder.h>
#include <condition_variable>
#include <logging/logger.h>
#include <wpcpp/metadata.h>
#include <systemd/sd-bus.h>

#include <sdcpp/bus.h>
#include <sdcpp/error.h>

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
          add_output_port("osc", "8 bit raw midi").add_signal_processor(
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

  std::thread pw_thread([&builder]() {
    auto filter_app = builder.build();
    if (filter_app.has_value()) {
      filter_app.value()->run();
    }
  });

  struct WirePlumberControl {
    GOptionContext *context = nullptr;
    GMainLoop *loop = nullptr;
    WpCore *core = nullptr;
    WpObjectManager *object_manager = nullptr;
  };

  std::atomic<bool> initialized = false;
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

  while (true) {
    char const *line = repl.input(prompt);
    if (line == nullptr) {
      break;
    }

    std::string_view command(line);
    if (command == "exit") {
      break;
    } else if (command == "help") {
      std::cout.flush();
      std::cout << "pmx console v0.1" << std::endl;
      std::cout << "ask ford or try 42!" << std::endl;
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
          auto result = console::describe_command(iss, *proxy_collection,
                                                  *link_collection);
        } else if (token == "meta") {
          repl.history_add(line);
          auto result = console::meta_command(iss, metadata);
          if (!result) {
            std::cout << "There was an error: " << result.error().message <<
              std::endl;
          }
        } else if (token == "status") {
          repl.history_add(line);
          auto result = console::status_command(iss, bus);
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
