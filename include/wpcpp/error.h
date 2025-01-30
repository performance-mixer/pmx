#pragma once

#include <string>

namespace wpcpp {

enum class error_type { NOT_IMPLEMENTED };

struct error {
  std::string message;
  error_type type;

  static error not_implemented() {
    return {"Not implemented", error_type::NOT_IMPLEMENTED};
  }

};

} // namespace wpcpp