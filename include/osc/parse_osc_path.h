#pragma once

#include <cstddef>
#include <string>

#include "parameters/parameters.h"

namespace osc {

struct osc_path {
  size_t channel;
  std::shared_ptr<parameters::parameter> parameter;
};

osc_path parse_osc_path(const std::string &path);

} // namespace osc
