#include <gtest/gtest.h>

#include <ump/parsing.h>
#include <ump/note.h>
#include <ump/control_change.h>

TEST(UmpMidiParsingErrors, UnknownMidiMessageA) {
  const auto message = ump::parse(0, 0);
  ASSERT_FALSE(message.has_value());
  ASSERT_EQ(message.error().type, ump::error_type::UNKNOWN_MIDI_MESSAGE);
}

TEST(UmpMidiParsingErrors, UnknownMidiMessageB) {
  const auto message = ump::parse(0);
  ASSERT_FALSE(message.has_value());
  ASSERT_EQ(message.error().type, ump::error_type::UNKNOWN_MIDI_MESSAGE);
}

TEST(UmpMidiParsingErrors, MissingSecondValue) {
  const auto message = ump::parse(4 << 28);
  ASSERT_FALSE(message.has_value());
  ASSERT_EQ(message.error().type, ump::error_type::MISSING_SECOND_VALUE);
}
