#pragma once

#include <optional>
#include <pwcpp/filter/parameter.h>

namespace tools {
template <typename T>
std::optional<T> get_from_collection(size_t index,
                                     const std::span<pwcpp::filter::Parameter> &
                                     parameters) {
  auto variant = std::find_if(parameters.begin(), parameters.end(),
                              [&index](const auto &parameter) {
                                return parameter.id == index;
                              })->value;
  if (std::holds_alternative<T>(variant)) {
    return std::get<T>(variant);
  }

  return std::nullopt;
}
}
