#pragma once

#include <string>

namespace error {
enum class error_type {
  NOT_IMPLEMENTED,
  INVALID_ARGUMENT,
  SYSTEMD_CALL_METHOD,
  PIPEWIRE_ERROR,
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

  static error systemd(const std::string &message) {
    return {message, error_type::SYSTEMD_CALL_METHOD};
  }

  static error pipewire(const std::string &message) {
    return {message, error_type::PIPEWIRE_ERROR};
  }
};
}
