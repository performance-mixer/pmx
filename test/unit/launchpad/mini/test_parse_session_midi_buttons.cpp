#include <gtest/gtest.h>

#include <slp_ctrl/launchpad/mini/parse_session_midi.h>
#include <ump/messages.h>
#include <ump/note.h>

#include <tuple>

#include "ump/control_change.h"

class ParseButtonNoteEvents : public testing::TestWithParam<std::tuple<
    uint8_t, uint8_t, uint8_t>> {};

TEST_P(ParseButtonNoteEvents, ParseMidiNoteOnV2) {
  auto parameter = GetParam();
  auto index = std::get<0>(parameter);
  auto row = std::get<1>(parameter);
  auto column = std::get<2>(parameter);

  auto message = ump::v2::note_on_message(0, 0, index,
                                          std::numeric_limits<uint16_t>::max(),
                                          ump::messages::v2::attribute_type::none,
                                          0);

  ASSERT_TRUE(message.has_value());

  auto first = message.value()[0];
  auto second = message.value()[1];
  auto event = slp::ctrl::launchpad::mini::parse_session_midi(first, second);

  ASSERT_TRUE(event.has_value());

  auto variant = event.value();
  ASSERT_TRUE(
    std::holds_alternative<slp::ctrl::launchpad::mini::session::button_down>(
      variant));

  const auto button_down_event = std::get<
    slp::ctrl::launchpad::mini::session::button_down>(variant);
  ASSERT_EQ(button_down_event.row, row);
  ASSERT_EQ(button_down_event.column, column);
}

TEST_P(ParseButtonNoteEvents, ParseMidiNodeOffV2) {
  auto parameter = GetParam();
  auto index = std::get<0>(parameter);
  auto row = std::get<1>(parameter);
  auto column = std::get<2>(parameter);

  auto message = ump::v2::note_off_message(0, 0, index,
                                           std::numeric_limits<uint16_t>::max(),
                                           ump::messages::v2::attribute_type::none,
                                           0);

  ASSERT_TRUE(message.has_value());

  auto first = message.value()[0];
  auto second = message.value()[1];
  auto event = slp::ctrl::launchpad::mini::parse_session_midi(first, second);

  ASSERT_TRUE(event.has_value());

  auto variant = event.value();
  ASSERT_TRUE(
    std::holds_alternative<slp::ctrl::launchpad::mini::session::button_up>(
      variant));

  const auto button_up_event = std::get<
    slp::ctrl::launchpad::mini::session::button_up>(variant);
  ASSERT_EQ(button_up_event.row, row);
  ASSERT_EQ(button_up_event.column, column);
}

TEST_P(ParseButtonNoteEvents, ParseMidiNodeOffV1) {
  auto parameter = GetParam();
  auto index = std::get<0>(parameter);
  auto row = std::get<1>(parameter);
  auto column = std::get<2>(parameter);

  auto message = ump::v1::note_off_message(0, 0, index,
                                           std::numeric_limits<uint8_t>::max());

  ASSERT_TRUE(message.has_value());

  auto first = message.value();
  auto event = slp::ctrl::launchpad::mini::parse_session_midi(first);

  ASSERT_TRUE(event.has_value());

  auto variant = event.value();
  ASSERT_TRUE(
    std::holds_alternative<slp::ctrl::launchpad::mini::session::button_up>(
      variant));

  const auto button_up_event = std::get<
    slp::ctrl::launchpad::mini::session::button_up>(variant);
  ASSERT_EQ(button_up_event.row, row);
  ASSERT_EQ(button_up_event.column, column);
}

TEST_P(ParseButtonNoteEvents, ParseMidiNoteOnV1) {
  auto parameter = GetParam();
  auto index = std::get<0>(parameter);
  auto row = std::get<1>(parameter);
  auto column = std::get<2>(parameter);

  auto message = ump::v1::note_on_message(0, 0, index,
                                          std::numeric_limits<uint8_t>::max());

  ASSERT_TRUE(message.has_value());

  auto first = message.value();
  auto event = slp::ctrl::launchpad::mini::parse_session_midi(first);

  ASSERT_TRUE(event.has_value());

  auto variant = event.value();
  ASSERT_TRUE(
    std::holds_alternative<slp::ctrl::launchpad::mini::session::button_down>(
      variant));

  const auto button_down_event = std::get<
    slp::ctrl::launchpad::mini::session::button_down>(variant);
  ASSERT_EQ(button_down_event.row, row);
  ASSERT_EQ(button_down_event.column, column);
}

INSTANTIATE_TEST_SUITE_P(SessionMode, ParseButtonNoteEvents,
                         testing::Values( std::make_tuple(81, 1, 0), std::
                           make_tuple(82, 1, 1), std:: make_tuple(83, 1, 2), std
                           ::make_tuple(84, 1, 3), std::make_tuple(85, 1, 4),
                           std::make_tuple(86, 1, 5), std::make_tuple(87, 1, 6),
                           std::make_tuple(88, 1, 7 ), std::make_tuple(71, 2, 0)
                           , std::make_tuple(72, 2, 1), std::make_tuple(73, 2, 2
                           ), std::make_tuple(74, 2, 3), std::make_tuple(75,2, 4
                           ), std::make_tuple( 76, 2, 5), std::make_tuple(77, 2,
                             6), std::make_tuple(78, 2, 7), std::make_tuple(61,
                             3, 0), std::make_tuple(62, 3, 1), std::make_tuple(
                             63, 3, 2), std::make_tuple(64, 3, 3),std::
                           make_tuple(65, 3, 4), std::make_tuple(66, 3, 5 ), std
                           ::make_tuple(67, 3, 6), std::make_tuple(68, 3, 7),
                           std::make_tuple(51, 4 , 0), std::make_tuple(52, 4, 1)
                           , std::make_tuple(53, 4, 2), std::make_tuple( 54, 4,
                             3), std::make_tuple(55, 4, 4), std::make_tuple( 56,
                             4, 5), std:: make_tuple(57, 4, 6), std::make_tuple(
                             58, 4, 7), std::make_tuple(41, 5, 0), std::
                           make_tuple(42, 5, 1), std::make_tuple(43, 5, 2),std::
                           make_tuple(44, 5, 3 ), std::make_tuple(45, 5, 4), std
                           ::make_tuple(46, 5, 5), std::make_tuple(47, 5 , 6),
                           std::make_tuple(48, 5, 7), std::make_tuple(31, 6, 0),
                           std::make_tuple( 32, 6, 1), std::make_tuple( 33, 6, 2
                           ), std::make_tuple(34, 6, 3), std::make_tuple(35, 6,
                             4), std::make_tuple(36, 6, 5), std::make_tuple(37,
                             6, 6), std::make_tuple(38, 6, 7),std::make_tuple(21
                             , 7, 0), std::make_tuple(22, 7, 1 ), std::
                           make_tuple(23, 7, 2), std::make_tuple(24, 7, 3), std
                           ::make_tuple(25, 7 , 4), std::make_tuple(26, 7, 5),
                           std::make_tuple(27, 7, 6), std::make_tuple( 28, 7, 7)
                           , std::make_tuple(11, 8, 0), std::make_tuple( 12, 8,
                             1), std:: make_tuple(13, 8, 2), std::make_tuple(14,
                             8, 3), std::make_tuple(15, 8, 4), std::make_tuple(
                             16, 8, 5), std::make_tuple(17, 8, 6),std::
                           make_tuple(18, 8, 7 )));

class ParseButtonControlChangeEvents : public testing::TestWithParam<std::tuple<
    uint8_t, uint8_t, uint8_t>> {};

TEST_P(ParseButtonControlChangeEvents, ParseMidiControlChangeV2Press) {
  auto parameter = GetParam();
  auto index = std::get<0>(parameter);
  auto row = std::get<1>(parameter);
  auto column = std::get<2>(parameter);

  auto message = ump::v2::control_change_message(0, 0, index,
                                                 std::numeric_limits<
                                                   uint32_t>::max());

  ASSERT_TRUE(message.has_value());

  auto first = message.value()[0];
  auto second = message.value()[1];
  auto event = slp::ctrl::launchpad::mini::parse_session_midi(first, second);

  ASSERT_TRUE(event.has_value());

  auto variant = event.value();
  ASSERT_TRUE(
    std::holds_alternative<slp::ctrl::launchpad::mini::session::button_down>(
      variant));

  const auto button_down_event = std::get<
    slp::ctrl::launchpad::mini::session::button_down>(variant);
  ASSERT_EQ(button_down_event.row, row);
  ASSERT_EQ(button_down_event.column, column);
}

TEST_P(ParseButtonControlChangeEvents, ParseMidiControlChangeV2Release) {
  auto parameter = GetParam();
  auto index = std::get<0>(parameter);
  auto row = std::get<1>(parameter);
  auto column = std::get<2>(parameter);

  auto message = ump::v2::control_change_message(0, 0, index, 0);

  ASSERT_TRUE(message.has_value());

  auto first = message.value()[0];
  auto second = message.value()[1];
  auto event = slp::ctrl::launchpad::mini::parse_session_midi(first, second);

  ASSERT_TRUE(event.has_value());

  auto variant = event.value();
  ASSERT_TRUE(
    std::holds_alternative<slp::ctrl::launchpad::mini::session::button_up>(
      variant));

  const auto button_down_event = std::get<
    slp::ctrl::launchpad::mini::session::button_down>(variant);
  ASSERT_EQ(button_down_event.row, row);
  ASSERT_EQ(button_down_event.column, column);
}

TEST_P(ParseButtonControlChangeEvents, ParseMidiControlChangeV1Press) {
  auto parameter = GetParam();
  auto index = std::get<0>(parameter);
  auto row = std::get<1>(parameter);
  auto column = std::get<2>(parameter);

  auto message = ump::v1::control_change_message(0, 0, index, 127);

  ASSERT_TRUE(message.has_value());

  auto first = message.value();
  auto event = slp::ctrl::launchpad::mini::parse_session_midi(first);

  ASSERT_TRUE(event.has_value());

  auto variant = event.value();
  ASSERT_TRUE(
    std::holds_alternative<slp::ctrl::launchpad::mini::session::button_down>(
      variant));

  const auto button_down_event = std::get<
    slp::ctrl::launchpad::mini::session::button_down>(variant);
  ASSERT_EQ(button_down_event.row, row);
  ASSERT_EQ(button_down_event.column, column);
}

TEST_P(ParseButtonControlChangeEvents, ParseMidiControlChangeV1Release) {
  auto parameter = GetParam();
  auto index = std::get<0>(parameter);
  auto row = std::get<1>(parameter);
  auto column = std::get<2>(parameter);

  auto message = ump::v1::control_change_message(0, 0, index, 0);

  ASSERT_TRUE(message.has_value());

  auto first = message.value();
  auto event = slp::ctrl::launchpad::mini::parse_session_midi(first);

  ASSERT_TRUE(event.has_value());

  auto variant = event.value();
  ASSERT_TRUE(
    std::holds_alternative<slp::ctrl::launchpad::mini::session::button_up>(
      variant));

  const auto button_down_event = std::get<
    slp::ctrl::launchpad::mini::session::button_down>(variant);
  ASSERT_EQ(button_down_event.row, row);
  ASSERT_EQ(button_down_event.column, column);
}

INSTANTIATE_TEST_SUITE_P(SessionMode, ParseButtonControlChangeEvents,
                         testing::Values( std::make_tuple(91, 0, 0), std::
                           make_tuple(92, 0, 1), std::make_tuple(93, 0, 2), std
                           ::make_tuple(94, 0, 3), std::make_tuple(95, 0, 4),
                           std::make_tuple(96, 0, 5), std::make_tuple(97, 0, 6),
                           std::make_tuple(98, 0, 7), std::make_tuple(99, 0, 8),
                           std::make_tuple(89, 1, 8), std::make_tuple(79, 2, 8),
                           std::make_tuple(69, 3, 8), std::make_tuple(59, 4, 8),
                           std::make_tuple(49, 5, 8), std::make_tuple(39, 6, 8),
                           std::make_tuple(29, 7, 8), std::make_tuple(19, 8, 8)
                         ));
