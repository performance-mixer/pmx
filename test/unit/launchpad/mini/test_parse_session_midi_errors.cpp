#include <gtest/gtest.h>

#include <slp_ctrl/launchpad/mini/parse_session_midi.h>
#include <ump/messages.h>
#include <ump/note.h>

class InvalidButtonIndex : public testing::TestWithParam<unsigned int> {};

TEST_P(InvalidButtonIndex, NoteOnV2) {
  auto index = GetParam();

  auto message = ump::v2::note_on_message(0, 0, index,
                                          std::numeric_limits<uint16_t>::max(),
                                          ump::messages::v2::attribute_type::none,
                                          0);

  ASSERT_TRUE(message.has_value());

  auto first = message.value()[0];
  auto second = message.value()[1];
  auto event = slp::ctrl::launchpad::mini::parse_session_midi(first, second);

  ASSERT_FALSE(event.has_value());
  ASSERT_EQ(event.error().type, error::error_type::INVALID_CC_INDEX);
}

INSTANTIATE_TEST_SUITE_P(SessionModeErrors, InvalidButtonIndex,
                         testing::Values(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 100,
                           101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111
                           , 112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
                           122, 123, 124, 125, 126, 127));
