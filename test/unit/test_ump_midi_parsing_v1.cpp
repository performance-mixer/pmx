#include <gtest/gtest.h>

#include <ump/parsing.h>
#include <ump/note.h>
#include <ump/control_change.h>

TEST(UmpMidiParsingV1, NoteOnA) {
  const auto note_on_message = ump::v1::note_on_message(1, 3, 34, 21);
  ASSERT_TRUE(note_on_message.has_value());
  const auto message = ump::parse(note_on_message.value(), 0x00);
  ASSERT_TRUE(message.has_value());
  const auto &variant = message.value();
  ASSERT_TRUE(
    std::holds_alternative<ump::messages::v1::note_on>(variant));
  const auto &note = std::get<ump::messages::v1::note_on>(variant);
  ASSERT_EQ(note.group, 1);
  ASSERT_EQ(note.channel, 3);
  ASSERT_EQ(note.note_number, 34);
  ASSERT_EQ(note.velocity, 21);
}

TEST(UmpMidiParsingV1, NoteOffA) {
  const auto note_off_message = ump::v1::note_off_message(1, 3, 34, 4);
  ASSERT_TRUE(note_off_message.has_value());
  const auto message = ump::parse(note_off_message.value());
  ASSERT_TRUE(message.has_value());
  const auto &variant = message.value();
  ASSERT_TRUE(
    std::holds_alternative<ump::messages::v1::note_off>(variant));
  const auto &note = std::get<ump::messages::v1::note_off>(variant);
  ASSERT_EQ(note.group, 1);
  ASSERT_EQ(note.channel, 3);
  ASSERT_EQ(note.note_number, 34);
  ASSERT_EQ(note.velocity, 4);
}

TEST(UmpMidiParsingV1, ControlChangeA) {
  const auto control_change_message = ump::v1::control_change_message(4, 3, 23, 32);
  ASSERT_TRUE(control_change_message.has_value());
  const auto message = ump::parse(control_change_message.value());
  ASSERT_TRUE(message.has_value());
  const auto &variant = message.value();
  ASSERT_TRUE(
    std::holds_alternative<ump::messages::v1::control_change>(variant));
  const auto &control_change = std::get<ump::messages::v1::control_change>(variant);
  ASSERT_EQ(control_change.group, 4);
  ASSERT_EQ(control_change.channel, 3);
  ASSERT_EQ(control_change.index, 23);
  ASSERT_EQ(control_change.value, 32);
}