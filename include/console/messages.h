#pragma once

#include <cstdint>
#include <string>

namespace console {
enum class command { send };

enum class sub_command { osc, midi };

struct request_message {
  console::command command;
  console::sub_command sub_command;
  std::uint8_t data[1024];
  std::size_t size;
};

enum class response_message_status { ok, error };

struct response_message {
  response_message_status status;
  std::string message;
};

}
