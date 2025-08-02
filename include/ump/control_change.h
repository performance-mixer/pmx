#pragma once
#include <expected>

#include "error.h"

namespace ump::v1 {
inline std::expected<unsigned int, error>
control_change_message(const unsigned int group, const unsigned int channel,
                       const unsigned int index, const unsigned int value) {
  unsigned int message_type_and_group = 0b00100000 | group;
  unsigned int v1_message_type_and_channel = 0b10110000 | channel;
  return message_type_and_group << 24 | v1_message_type_and_channel << 16 |
    index << 8 | value;
}
}

namespace ump::v2 {
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
