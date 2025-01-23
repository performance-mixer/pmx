#include "interpolation/interpolation.h"

double interpolation::interpolate(parameters::parameter &parameter,
                                  u_int8_t control_value) {
  auto min = parameter.min;
  auto max = parameter.max;
  return min + (max - min) * control_value / 127.0;
}
