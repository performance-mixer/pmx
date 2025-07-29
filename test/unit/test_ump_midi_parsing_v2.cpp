#include <gtest/gtest.h>

#include <ump/parsing.h>
#include <ump/note.h>
#include <ump/control_change.h>

TEST(UmpMidiParsingV2, NoteOnA) {
  const auto note_on_message = ump::v2::note_on_message(
    3, 5, 34, 11, ump::messages::v2::attribute_type::profile, 465);
  ASSERT_TRUE(note_on_message.has_value());
  const auto message = ump::parse(note_on_message.value()[0],
                                  note_on_message.value()[1]);
  ASSERT_TRUE(message.has_value());
  const auto &variant = message.value();
  ASSERT_TRUE(std::holds_alternative<ump::messages::v2::note_on>(variant));
  const auto &note = std::get<ump::messages::v2::note_on>(variant);
  ASSERT_EQ(note.group, 3);
  ASSERT_EQ(note.channel, 5);
  ASSERT_EQ(note.note_number, 34);
  ASSERT_EQ(note.velocity, 11);
  ASSERT_EQ(note.attribute_type, ump::messages::v2::attribute_type::profile);
  ASSERT_EQ(note.attribute_value, 465);
}

TEST(UmpMidiParsingV2, NoteOffA) {
  const auto note_off_message = ump::v2::note_off_message(
    6, 2, 54, 45, ump::messages::v2::attribute_type::manufacturer, 665);
  ASSERT_TRUE(note_off_message.has_value());
  const auto message = ump::parse(note_off_message.value()[0],
                                  note_off_message.value()[1]);
  ASSERT_TRUE(message.has_value());
  const auto &variant = message.value();
  ASSERT_TRUE(
    std::holds_alternative<ump::messages::v2::note_off>(variant));
  const auto &note = std::get<ump::messages::v2::note_off>(variant);
  ASSERT_EQ(note.group, 6);
  ASSERT_EQ(note.channel, 2);
  ASSERT_EQ(note.note_number, 54);
  ASSERT_EQ(note.velocity, 45);
  ASSERT_EQ(note.attribute_type, ump::messages::v2::attribute_type::manufacturer);
  ASSERT_EQ(note.attribute_value, 665);
}

TEST(UmpMidiParsingV2, ControlChangeA) {
  const auto control_change_message = ump::v2::control_change_message(1, 2, 3, 4);
  ASSERT_TRUE(control_change_message.has_value());
  const auto message = ump::parse(control_change_message.value()[0],
                                  control_change_message.value()[1]);
  ASSERT_TRUE(message.has_value());
  const auto &variant = message.value();
  ASSERT_TRUE(
    std::holds_alternative<ump::messages::v2::control_change>(variant));
  const auto &control_change = std::get<ump::messages::v2::control_change>(variant);
  ASSERT_EQ(control_change.group, 1);
  ASSERT_EQ(control_change.channel, 2);
  ASSERT_EQ(control_change.index, 3);
  ASSERT_EQ(control_change.value, 4);
}