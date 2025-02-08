#include "osc/parse_osc_path.h"
#include "parameters/parameters.h"

#include <microtest/microtest.h>

TEST(ParseAnInputChannelOscPath) {
  auto path = osc::parse_osc_path("/I/A/3/CMP/ratio");
  ASSERT_EQ(path.channel, 3);
  ASSERT_EQ(path.parameter, parameters::compressor_ratio);
}

TEST(ParseAnotherInputChannelOscPath) {
  auto path = osc::parse_osc_path("/I/A/13/SAT/level_in");
  ASSERT_EQ(path.channel, 13);
  ASSERT_EQ(path.parameter, parameters::saturator_level_in);
}

TEST_MAIN()
