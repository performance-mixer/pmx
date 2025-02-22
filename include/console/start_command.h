#pragma once
#include <pmx/constants.h>

namespace console {
inline std::expected<void, error::error> start_command(
  std::istringstream &stream, sdcpp::Bus &bus) {
  sdcpp::enable_units(bus, pmx::constants::service_names);
  return {};
}
}
