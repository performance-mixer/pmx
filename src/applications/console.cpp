#include "console/messages.h"
#include "console/send_command.h"

#include <iostream>
#include <replxx.hxx>
#include <sstream>
#include <pwcpp/filter/app_builder.h>
#include <condition_variable>

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

  repl.install_window_change_handler();

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
            continue;
          } else {
            std::cout << "There was an error: " << result.error().message <<
              std::endl;
            continue;
          }
        }

        std::cout << "Unknown command: " << token << std::endl;
      }
    }
  }

  std::cout << "Goodbye!" << std::endl;
}
