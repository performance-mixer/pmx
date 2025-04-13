#pragma once

#include <optional>

#include <pwcpp/property/property.h>

namespace tools {
template <typename T>
std::optional<T> get_from_collection(const std::string &name,
                                     const std::span<const std::tuple<
                                       std::string,
                                       pwcpp::property::property_value_type>> &
                                     parameters) {
  auto variant = std::get<1>(*std::find_if(parameters.begin(), parameters.end(),
                                           [&name](const auto &parameter) {
                                             return get<0>(parameter) == name;
                                           }));
  if (std::holds_alternative<T>(variant)) {
    return std::get<T>(variant);
  }

  return std::nullopt;
}
}
