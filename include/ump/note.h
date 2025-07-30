#pragma once

#include "error.h"
#include "messages.h"

#include <expected>

namespace ump::v1 {
inline std::expected<unsigned int, error>
note_on_message(const unsigned int group, const unsigned int channel,
                const unsigned int note_number, const unsigned int velocity) {
  unsigned int message_type_and_group = 0b00100000 | group;
  unsigned int message_type_and_channel = 0b10010000 | channel;
  return message_type_and_group << 24 | message_type_and_channel << 16 |
    note_number << 8 | velocity;
}

inline std::expected<unsigned int, error>
note_off_message(const unsigned int group, const unsigned int channel,
                 const unsigned int note_number, const unsigned int velocity) {
  unsigned int message_type_and_group = 0b00100000 | group;
  unsigned int message_type_and_channel = 0b10000000 | channel;
  return message_type_and_group << 24 | message_type_and_channel << 16 |
    note_number << 8 | velocity;
}
}

namespace ump::v2 {
inline std::expected<std::array<unsigned int, 2>, error> note_off_message(
  const unsigned int group, const unsigned int channel,
  const unsigned int note_number, const unsigned int velocity,
  messages::v2::attribute_type attribute_type, unsigned int attribute_value) {
  const unsigned int message_type_and_group = 0b01000000 | group;
  const unsigned int message_type_and_channel = 0b10000000 | channel;
  unsigned int msg_attribute_type = 0;
  if (attribute_type == messages::v2::attribute_type::manufacturer) {
    msg_attribute_type = 1;
  } else if (attribute_type == messages::v2::attribute_type::profile) {
    msg_attribute_type = 2;
  } else if (attribute_type == messages::v2::attribute_type::pitch) {
    msg_attribute_type = 3;
  }
  const unsigned int note_number_and_attribute_type = note_number << 8 |
    msg_attribute_type;
  const unsigned int first = message_type_and_group << 24 |
    message_type_and_channel << 16 | note_number_and_attribute_type;

  const unsigned int second = velocity << 16 | attribute_value;
  return {{first, second}};
}

inline std::expected<std::array<unsigned int, 2>, error> note_on_message(
  const unsigned int group, const unsigned int channel,
  const unsigned int note_number, const unsigned int velocity,
  messages::v2::attribute_type attribute_type, unsigned int attribute_value) {
  const unsigned int message_type_and_group = 0b01000000 | group;
  const unsigned int message_type_and_channel = 0b10010000 | channel;
  unsigned int msg_attribute_type = 0;
  if (attribute_type == messages::v2::attribute_type::manufacturer) {
    msg_attribute_type = 1;
  } else if (attribute_type == messages::v2::attribute_type::profile) {
    msg_attribute_type = 2;
  } else if (attribute_type == messages::v2::attribute_type::pitch) {
    msg_attribute_type = 3;
  }
  const unsigned int note_number_and_attribute_type = note_number << 8 |
    msg_attribute_type;
  const unsigned int first = message_type_and_group << 24 |
    message_type_and_channel << 16 | note_number_and_attribute_type;

  const unsigned int second = velocity << 16 | attribute_value;
  return {{first, second}};
}

inline std::expected<std::array<unsigned int, 2>, error> control_change_message(
  const unsigned int group, const unsigned int channel,
  const unsigned int index, const unsigned int value) {
  const unsigned int message_type_and_group = 0b01000000 | group;
  const unsigned int message_type_and_channel = 0b10110000 | channel;
  const unsigned int first = message_type_and_group << 24 |
    message_type_and_channel << 16 | index << 8;
  return {{first, value}};
}
}
