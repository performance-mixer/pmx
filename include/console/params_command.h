#pragma once

#include <proxy/proxy_watcher.h>

#include "tools/pipewire_tools.h"

namespace console {
inline std::expected<void, error::error> params_command(
  std::istringstream &stream, proxy::ProxyWatcher &proxy_watcher,
  pw_main_loop *loop) {
  std::string object;
  if (stream >> object) {
    if (std::all_of(object.begin(), object.end(), ::isdigit)) {
      return {};
    } else {
      return std::unexpected(
        error::error::invalid_argument("node id should be a positive number"));
    }
  } else {
    return std::unexpected(error::error::invalid_argument("missing node id"));
  }
}
}
