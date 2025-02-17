#pragma once

#include <expected>

#include "console/messages.h"
#include "error/error.h"

#include <fstream>
#include <sstream>
#include <oscpp/client.hpp>

namespace console {
inline std::expected<void, error::error> send_command(
  std::istringstream &stream, request_message &message) {
  std::string sub_command_string;
  if (stream >> sub_command_string) {
    if (sub_command_string != "osc") {
      return std::unexpected(
        error::error::invalid_argument(
          "unsupported sub-command " + sub_command_string));
    }
  } else {
    return std::unexpected(
      error::error::invalid_argument("missing sub-command"));
  }

  std::string path;
  if (stream >> path) {
    std::string message_descriptor;
    if (stream >> message_descriptor) {
      std::string value_string;
      if (message_descriptor == "f") {
        if (stream >> value_string) {
          float value;
          auto result = std::from_chars(value_string.data(),
                                        value_string.data() + value_string.
                                        size(), value);
          if (result.ec == std::errc()) {
            OSCPP::Client::Packet packet(message.data, 1024);
            packet.openMessage(path.c_str(), 1).float32(value).closeMessage();
            message.size = packet.size();
            message.command = command::send;
            message.sub_command = sub_command::osc;
            return {};
          }

          return std::unexpected(
            error::error::invalid_argument("invalid value"));
        }
      } else {
        return std::unexpected(
          error::error::invalid_argument(
            "not implemented: " + message_descriptor));
      }
    } else {
      return std::unexpected(
        error::error::invalid_argument("missing message descriptor"));
    }
  }

  return std::unexpected(error::error::invalid_argument("missing path"));
};
}
