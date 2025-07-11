#pragma once
#include <expected>

#include "error/error.h"

namespace ump {
inline std::expected<unsigned int, error::error>
note_on_message(const unsigned int group, const unsigned int channel,
                       const unsigned int note_number,
                       const unsigned int velocity) {
  unsigned int message_type_and_group = 0b00100000 | group;
  unsigned int v1_message_type_and_channel = 0b10010000 | channel;
  return message_type_and_group << 24 | v1_message_type_and_channel << 16 |
    note_number << 8 | velocity;
}
}
