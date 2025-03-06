#include "interpolation/interpolation.h"

#include <gtest/gtest.h>

TEST(Interpolation, LinearInterpolation) {
  const auto parameter = parameters::parameter(1, "param", "CMP", 2.3, 5.6, "CMP");
  const auto lowest_value = interpolation::interpolate(parameter, 0);
  ASSERT_EQ(lowest_value, 2.3);

  const auto highest_value = interpolation::interpolate(parameter, 127);
  ASSERT_EQ(highest_value, 5.6);

  const auto mid_point = interpolation::interpolate(parameter, 64);
  ASSERT_TRUE(3.96 < mid_point && mid_point < 3.97);
}
