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
      auto id = std::stoi(object);
      auto proxy = proxy_watcher.get_proxy(id);
      if (proxy.has_value()) {
        std::cout << proxy->name << std::endl << std::endl << "Parameters:" <<
          std::endl;
        auto parameters = proxy->parameters();
        for (const auto &parameter : parameters) {
          std::cout << std::get<0>(parameter) << ": ";
          auto value = std::get<1>(parameter);
          if (std::holds_alternative<std::string>(value)) {
            std::cout << std::get<std::string>(value) << std::endl;
          } else if (std::holds_alternative<int>(value)) {
            std::cout << std::get<int>(value) << std::endl;
          } else if (std::holds_alternative<float>(value)) {
            std::cout << std::get<float>(value) << std::endl;
          } else if (std::holds_alternative<double>(value)) {
            std::cout << std::get<double>(value) << std::endl;
          } else {
            std::cout << "unknown type" << std::endl;
          }
        }
      }
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
