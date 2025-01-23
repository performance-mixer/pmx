#pragma once

#include "parameters/parameters.h"

#include <sys/types.h>

namespace interpolation {

double interpolate(parameters::parameter &parameter, u_int8_t control_value);

} // namespace interpolation
