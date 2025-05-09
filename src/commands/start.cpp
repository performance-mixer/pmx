
#include <pmx/constants.h>
#include <sdcpp/units.h>

#include "console/commands.h"

std::expected<void, sdcpp::error> console::start_command(sdcpp::Bus &bus) {
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
