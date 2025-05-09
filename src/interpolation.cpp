#include "interpolation/interpolation.h"

#include <limits>

double interpolation::interpolate(const parameters::parameter &parameter,
                                  const uint32_t control_value) {
  const auto min = parameter.min;
  const auto max = parameter.max;
  return min + (max - min) * control_value / std::numeric_limits<
    uint32_t>::max();
}
