#pragma once
#include <pmx/constants.h>

namespace console {
inline std::expected<void, sdcpp::error> start_command(
  std::istringstream &stream, sdcpp::Bus &bus) {
  auto result = enable_units(bus, pmx::constants::service_names);
  if (!result.has_value()) {
    return std::unexpected(result.error());
  }

  result = start_units(bus, pmx::constants::service_names);
  if (!result.has_value()) {
    return std::unexpected(result.error());
  }

  return {};
}
}
