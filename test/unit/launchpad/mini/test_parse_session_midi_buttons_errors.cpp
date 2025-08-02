#include <gtest/gtest.h>

#include <slp_ctrl/launchpad/mini/parse_session_midi.h>
#include <ump/messages.h>
#include <ump/note.h>

#include "ump/control_change.h"

class InvalidButtonIndex : public testing::TestWithParam<unsigned int> {};

TEST_P(InvalidButtonIndex, NoteOnV2) {
  const auto note_number = GetParam();

  const auto message = ump::v2::note_on_message(0, 0, note_number,
                                                std::numeric_limits<
                                                  uint16_t>::max(),
                                                ump::messages::v2::attribute_type::none,
                                                0);

  ASSERT_TRUE(message.has_value());

  const auto first = message.value()[0];
  const auto second = message.value()[1];
  const auto event = slp::ctrl::launchpad::mini::parse_session_midi(
    first, second);

  ASSERT_FALSE(event.has_value());
  ASSERT_EQ(event.error().type, error::error_type::INVALID_NOTE_NUMBER);
}

TEST_P(InvalidButtonIndex, NoneOnV1) {
  const auto note_number = GetParam();

  const auto message = ump::v1::note_on_message(0, 0, note_number,
                                                std::numeric_limits<
                                                  uint8_t>::max());

  ASSERT_TRUE(message.has_value());

  const auto first = message.value();
  const auto event = slp::ctrl::launchpad::mini::parse_session_midi(first);

  ASSERT_FALSE(event.has_value());
  ASSERT_EQ(event.error().type, error::error_type::INVALID_NOTE_NUMBER);
}

TEST_P(InvalidButtonIndex, NoneOffV2) {
  const auto note_number = GetParam();

  const auto message = ump::v2::note_off_message(0, 0, note_number,
                                                 0,
                                                 ump::messages::v2::attribute_type::none,
                                                 0);

  ASSERT_TRUE(message.has_value());

  const auto first = message.value()[0];
  const auto second = message.value()[1];
  const auto event = slp::ctrl::launchpad::mini::parse_session_midi(
    first, second);

  ASSERT_FALSE(event.has_value());
  ASSERT_EQ(event.error().type, error::error_type::INVALID_NOTE_NUMBER);
}

TEST_P(InvalidButtonIndex, NoneOffV1) {
  const auto note_number = GetParam();

  const auto message = ump::v1::note_off_message(0, 0, note_number, 0);

  ASSERT_TRUE(message.has_value());

  const auto first = message.value();
  const auto event = slp::ctrl::launchpad::mini::parse_session_midi(first);

  ASSERT_FALSE(event.has_value());
  ASSERT_EQ(event.error().type, error::error_type::INVALID_NOTE_NUMBER);
}

INSTANTIATE_TEST_SUITE_P(SessionModeErrors, InvalidButtonIndex,
                         testing::Values(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 19,
                           20, 29, 30, 39, 40, 49, 50 , 59, 60, 69, 70, 79, 80,
                           89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101
                           , 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
                           112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122
                           , 123, 124, 125, 126, 127));

class InvalidControlChangeIndex : public testing::TestWithParam<unsigned int> {
};

TEST_P(InvalidControlChangeIndex, ControlChangeV2) {
  const auto invalid_index = GetParam();

  const auto message = ump::v2::control_change_message(0, 0, invalid_index,
    std::numeric_limits<uint16_t>::max());

  ASSERT_TRUE(message.has_value());

  const auto first = message.value()[0];
  const auto second = message.value()[1];
  const auto event = slp::ctrl::launchpad::mini::parse_session_midi(
    first, second);

  ASSERT_FALSE(event.has_value());
  ASSERT_EQ(event.error().type,
            error::error_type::INVALID_CONTROL_CHANGE_INDEX);
}

TEST_P(InvalidControlChangeIndex, ControlChangeV1) {
  const auto invalid_index = GetParam();

  const auto message =
    ump::v1::control_change_message(0, 0, invalid_index, 127);

  ASSERT_TRUE(message.has_value());

  const auto first = message.value();
  const auto event = slp::ctrl::launchpad::mini::parse_session_midi(first);

  ASSERT_FALSE(event.has_value());
  ASSERT_EQ(event.error().type,
            error::error_type::INVALID_CONTROL_CHANGE_INDEX);
}

INSTANTIATE_TEST_SUITE_P(SessionModeErrors, InvalidControlChangeIndex,
                         testing::Values(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                           12, 13, 14, 15, 16, 17, 18, 20, 21, 22, 23, 24, 25,
                           26, 27, 28, 30, 31, 32, 33, 34, 35, 36, 37, 38, 40,
                           41, 42, 43, 44, 45, 46, 47, 48, 50, 51, 52, 53, 54,
                           55, 56, 57, 58, 60, 61, 62, 63, 64, 65, 66, 67, 68,
                           70, 71, 72, 73, 74, 75, 76, 77, 78, 80, 81, 82, 83,
                           84, 85, 86, 87, 88, 90, 100, 101, 102, 103, 104, 105,
                           106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116
                           , 117, 118, 119, 120, 121, 122, 123, 124, 125, 126,
                           127));
