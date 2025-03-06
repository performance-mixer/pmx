#include "console/commands.h"

#include <iostream>

#include <pwcpp/property/property.h>

std::expected<void, error::error> console::watch_command(
  std::istringstream &stream, proxy::ProxyWatcher &proxy_watcher) {
  std::string object;
  if (stream >> object) {
    if (std::all_of(object.begin(), object.end(), ::isdigit)) {
      const auto id = std::stoi(object);

      auto pw_proxy = proxy_watcher.get_proxy(id);
      if (pw_proxy.has_value()) {
        auto result = pw_proxy.value()->watch_proxy_prop_params(
          [](std::span<std::tuple<
               std::string, pwcpp::property::property_value_type>> updates,
             auto &proxy) {
            for (auto &update : updates) {
              std::cout << std::get<0>(update) << ": " << std::get<1>(update) <<
                std::endl;
            }
          });
        std::cout << "watching " << pw_proxy.value()->name << std::endl;
      } else {
        return std::unexpected(
          error::error::invalid_argument(
            "couldn't find proxy with id " + object));
      }
    }
  }

  return {};
}
