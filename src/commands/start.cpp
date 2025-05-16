#include <thread>
#include <chrono>

#include <pmx/constants.h>
#include <sdcpp/units.h>

#include "console/commands.h"

std::expected<void, sdcpp::error> console::start_command(sdcpp::Bus &bus) {
  /*
   * sooperlooper needs to start first, to simplify connection handling logic
   */
  const std::array<std::string, 1> sooperlooper_service_name{
    "pmx-sooperlooper.service"
  };
  auto result = enable_units(bus, sooperlooper_service_name);
  if (!result.has_value()) {
    return std::unexpected(result.error());
  }

  result = start_units(bus, sooperlooper_service_name);
  if (!result.has_value()) {
    return std::unexpected(result.error());
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));

  result = enable_units(bus, pmx::constants::service_names);
  if (!result.has_value()) {
    return std::unexpected(result.error());
  }

  result = start_units(bus, pmx::constants::service_names);
  if (!result.has_value()) {
    return std::unexpected(result.error());
  }

  return {};
}
