#pragma once

#include <pmx/constants.h>

#include "console/colors.h"

#include <sdcpp/units.h>

namespace console {
inline std::expected<void, sdcpp::error> status_command(
  std::istringstream &stream, sdcpp::Bus &bus) {
  auto units = sdcpp::list_units(bus);
  auto unit_files = sdcpp::list_unit_files(bus);
  if (units.has_value() && unit_files.has_value()) {
    for (auto &unit_name : pmx::constants::all_services) {
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
        } else if (unit->active_state == "failed") {
          color = colors::red;
        }

        std::cout << color << unit->active_state << reset_color << std::endl;
      } else {
        auto unit_file = std::find_if(unit_files.value().begin(),
                                      unit_files.value().end(),
                                      [unit_name](const auto &unit_file) {
                                        return unit_file.name.ends_with(
                                          unit_name);
                                      });
        if (unit_file != unit_files->end()) {
          std::string color;
          if (unit_file->status == "disabled") {
            color = colors::red;
          } else {
            color = colors::yellow;
          }
          std::cout << unit_name << " " << color << " " << unit_file->status <<
            reset_color << std::endl;
        } else {
          std::cout << unit_name << " " << colors::red << "not found" <<
            reset_color << std::endl;
        }
      }
    }

    return {};
  } else {
    if (!units.has_value()) {
      return std::unexpected(units.error());
    } else {
      return std::unexpected(unit_files.error());
    }
  }
}
}
