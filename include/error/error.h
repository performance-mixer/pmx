#pragma once

#include <string>

namespace error {
enum class error_type {
  NOT_IMPLEMENTED,
  INVALID_ARGUMENT,
  SYSTEMD_CALL_METHOD,
  PIPEWIRE_ERROR,
  INVALID_CC_INDEX,
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

  static error invalid_cc_index(const unsigned int index) {
    return {"Invalid CC index " + index, error_type::INVALID_CC_INDEX};
  }
};
}
