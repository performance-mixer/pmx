#pragma once

#include <expected>

#include "sdcpp/bus.h"
#include "sdcpp/error.h"

#include <string>
#include <vector>
#include <cstdint>

namespace sdcpp {
struct unit {
  std::string name;
  std::string description;
  std::string load_state;
  std::string active_state;
  std::string sub_state;
  std::string following;
  std::string object_path;
  std::uint32_t id;
  std::string job_type;
  std::string job_path;
};

std::expected<std::vector<unit>, error> list_units(Bus &bus);
}
