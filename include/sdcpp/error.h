#pragma once

#include <string>

namespace sdcpp {
enum class error_type {
  NOT_IMPLEMENTED,
  INVALID_ARGUMENT,
  SYSTEMD_CALL_METHOD,
  SYSTEMD_METHOD_PARSING_ERROR,
  SYSTEMD_MESSAGE_BUILDING_ERROR,
};

struct error {
  std::string message;
  error_type type;

  static error not_implemented() {
    return {"Not implemented", error_type::NOT_IMPLEMENTED};
  }

  static error invalid_argument(const std::string &message) {
    return {message, error_type::INVALID_ARGUMENT};
  }

  static error systemd_call_method(const std::string &message) {
    return {message, error_type::SYSTEMD_CALL_METHOD};
  }

  static error systemd_method_parsing_error(const std::string &message) {
    return {message, error_type::SYSTEMD_METHOD_PARSING_ERROR};
  }

  static error systemd_message_building_error(const std::string &message) {
    return {message, error_type::SYSTEMD_MESSAGE_BUILDING_ERROR};
  }
};
}
