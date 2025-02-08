#include "interpolation/interpolation.h"

double interpolation::interpolate(const parameters::parameter &parameter,
                                  const u_int8_t control_value) {
  const auto min = parameter.min;
  const auto max = parameter.max;
  return min + (max - min) * control_value / 127.0;
}
