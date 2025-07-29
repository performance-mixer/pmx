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
