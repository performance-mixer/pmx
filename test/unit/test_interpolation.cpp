#include "interpolation/interpolation.h"

#include <gtest/gtest.h>

TEST(Interpolation, LinearInterpolation) {
  const auto parameter = parameters::parameter(1, "param", "CMP", 2.3, 5.6,
                                               "CMP");
  const auto lowest_value = interpolation::interpolate(parameter, 0);
  ASSERT_EQ(lowest_value, 2.3);

  const auto highest_value = interpolation::interpolate(
    parameter, std::numeric_limits<uint32_t>::max());
  ASSERT_EQ(highest_value, 5.6);

  const auto mid_point = interpolation::interpolate(parameter,
                                                    std::numeric_limits<
                                                      uint32_t>::max() / 2);
  ASSERT_TRUE(3.949 < mid_point && mid_point < 3.96);
}
