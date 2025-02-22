#pragma once

#include "console/colors.h"

#include <sdcpp/units.h>

namespace console {
inline std::expected<void, sdcpp::error> status_command(
  std::istringstream &stream, sdcpp::Bus &bus) {
  auto names = {
    "pipewire.service", "wireplumber.service", "pmx-filter-chain-ctrl.service",
    "pmx-grpc-api.service", "pmx-metadata-manager.service",
    "pmx-midi-router.service", "pmx-osc-network-receiver.service",
    "pmx-osc-network-sender.service", "pmx-traktor-z1-router.service"
  };

  auto units = sdcpp::list_units(bus);
  if (units.has_value()) {
    for (auto &unit_name : names) {
      auto unit = std::find_if(units.value().begin(), units.value().end(),
                               [unit_name](const auto &unit) {
                                 return unit.name == unit_name;
                               });
      std::string reset_color = colors::reset;
      if (unit != units->end()) {
        std::cout << unit->name << " ";

        std::string color;
        if (unit->active_state == "active") {
          color = colors::green;
        }

        std::cout << color << unit->active_state << reset_color << std::endl;
      } else {
        auto color = colors::red;
        std::cout << unit_name << " " << color << "not found" << reset_color <<
          std::endl;
      }
    }

    return {};
  }

  return std::unexpected(units.error());
}
}
