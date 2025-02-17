#pragma once

#include <cstddef>
#include <expected>
#include <string>
#include <error/error.h>

#include "parameters/parameters.h"

namespace osc {
enum class osc_path_channel_type {
  input,
  group,
  layer,
  none
};

enum class osc_path_layer { A, B };

inline std::ostream &operator<<(std::ostream &os,
                                const osc_path_channel_type &type) {
  switch (type) {
  case osc_path_channel_type::input:
    os << "input";
    break;
  case osc_path_channel_type::group:
    os << "group";
    break;
  case osc_path_channel_type::layer:
    os << "layer";
    break;
  default:
    os << "unknown";
    break;
  }
  return os;
}

struct osc_path {
  size_t channel_id;
  osc_path_channel_type channel_type;
  osc_path_layer layer;
  std::shared_ptr<parameters::parameter> parameter;
};

std::expected<osc::osc_path, error::error> parse_osc_path(
  const std::string &path);
} // namespace osc
