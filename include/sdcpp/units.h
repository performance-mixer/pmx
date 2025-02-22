#pragma once

#include <expected>

#include "sdcpp/bus.h"
#include "sdcpp/error.h"

#include <string>
#include <vector>
#include <cstdint>
#include <ostream>

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

struct unit_file {
  std::string name;
  std::string status;
};


inline std::ostream &operator<<(std::ostream &os, const unit_file &uf) {
  os << uf.name << ": " << uf.status;
  return os;
}

std::expected<std::vector<unit_file>, error> list_unit_files(Bus &bus);

std::expected<void, error> enable_units(Bus &bus,
                                        const std::span<const std::string> &
                                        unit_names);

std::expected<void, error> start_units(Bus &bus,
                                       std::span<std::string> &unit_names);
}
