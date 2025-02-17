#pragma once

#include <string>

namespace error {
enum class error_type { NOT_IMPLEMENTED, INVALID_ARGUMENT };

struct error {
  std::string message;
  error_type type;

  static error not_implemented() {
    return {"Not implemented", error_type::NOT_IMPLEMENTED};
  }

  static error invalid_argument(const std::string &message) {
    return {message, error_type::INVALID_ARGUMENT};
  }
};
}
