#pragma once

#include "parameters/parameters.h"

#include <sys/types.h>

namespace interpolation {
double interpolate(const parameters::parameter &parameter,
                   uint32_t control_value);
} // namespace interpolation
