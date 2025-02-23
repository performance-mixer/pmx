#pragma once
#include <pmx/constants.h>

namespace console {
inline std::expected<void, sdcpp::error> start_command(
  std::istringstream &stream, sdcpp::Bus &bus) {
  auto result = sdcpp::enable_units(bus, pmx::constants::service_names);
  if (!result.has_value()) {
    return std::unexpected(result.error());
  }

  return {};
}
}
