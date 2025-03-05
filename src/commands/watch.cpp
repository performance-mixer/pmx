#include "console/commands.h"

#include <iostream>

#include <pwcpp/property/property.h>

template <typename VType>
inline void print(std::ostream &os, const VType &value) {
  if (std::holds_alternative<int>(value)) {
    os << std::get<int>(value);
  } else if (std::holds_alternative<float>(value)) {
    os << std::get<float>(value);
  } else if (std::holds_alternative<double>(value)) {
    os << std::get<double>(value);
  } else if (std::holds_alternative<std::string>(value)) {
    os << std::get<std::string>(value);
  } else if (std::holds_alternative<bool>(value)) {
    os << (std::get<bool>(value) ? "true" : "false");
  } else if (std::holds_alternative<std::nullopt_t>(value)) {
    os << "nullopt";
  }
}

std::ostream &operator<<(std::ostream &os,
                         const pwcpp::property::property_value_type &value) {
  print(os, value);
  return os;
}

std::expected<void, error::error> console::watch_command(
  std::istringstream &stream, proxy::ProxyWatcher &proxy_watcher) {
  std::string object;
  if (stream >> object) {
    if (std::all_of(object.begin(), object.end(), ::isdigit)) {
      auto id = std::stoi(object);

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
