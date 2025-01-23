#include "interpolation/interpolation.h"

#include <microtest/microtest.h>

TEST(SimpleLinearInterpolation) {
  auto parameter = parameters::parameter(1, "param", "CMP", 2.3, 5.6, "CMP");
  auto lowest_value = interpolation::interpolate(parameter, 0);
  ASSERT_EQ(lowest_value, 2.3);

  auto highest_value = interpolation::interpolate(parameter, 127);
  ASSERT_EQ(highest_value, 5.6);

  auto mid_point = interpolation::interpolate(parameter, 64);
  ASSERT_EQ(mid_point, 4.0);
}

TEST_MAIN()
