#include "osc/parse_osc_path.h"
#include "parameters/parameters.h"

#include <gtest/gtest.h>

TEST(ParseOscMessages, ParseInputMessage01) {
  auto path = osc::parse_osc_path("/I/A/3/CMP/ratio");
  ASSERT_EQ(path->channel_id, 3);
  ASSERT_EQ(path->parameter, parameters::compressor_ratio);
  ASSERT_EQ(path->channel_type, osc::osc_path_channel_type::input);
  ASSERT_EQ(path->layer, osc::osc_path_layer::A);
}

TEST(ParseOscMessages, ParseInputMessage02) {
  auto path = osc::parse_osc_path("/I/A/13/SAT/level_in");
  ASSERT_EQ(path->channel_id, 13);
  ASSERT_EQ(path->parameter, parameters::saturator_level_in);
  ASSERT_EQ(path->layer, osc::osc_path_layer::A);
}

TEST(ParseOscMessages, ParseInputMessage03) {
  auto path = osc::parse_osc_path("/I/B/13/SAT/level_in");
  ASSERT_EQ(path->channel_id, 13);
  ASSERT_EQ(path->parameter, parameters::saturator_level_in);
  ASSERT_EQ(path->layer, osc::osc_path_layer::B);
}

TEST(ParseOscMessages, ParseGroupMessages01) {
  auto path = osc::parse_osc_path("/G/A/1/CMP/ratio");
  ASSERT_EQ(path->channel_id, 1);
  ASSERT_EQ(path->parameter, parameters::compressor_ratio);
  ASSERT_EQ(path->channel_type, osc::osc_path_channel_type::group);
  ASSERT_EQ(path->layer, osc::osc_path_layer::A);
}

TEST(ParseOscMessages, ParseLayerMessages01) {
  auto path = osc::parse_osc_path("/L/B/4/CMP/ratio");
  ASSERT_EQ(path->channel_id, 4);
  ASSERT_EQ(path->parameter, parameters::compressor_ratio);
  ASSERT_EQ(path->channel_type, osc::osc_path_channel_type::layer);
  ASSERT_EQ(path->layer, osc::osc_path_layer::B);
}
