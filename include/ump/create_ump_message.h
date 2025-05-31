#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <expected>

#include "error/error.h"

namespace ump {
enum class message_type {
  sysex_7bit_start, sysex_7bit_continue, sysex_7bit_end,
};

inline std::expected<std::array<unsigned int, 2>, error::error>
create_8_byte_message(message_type type,
                      std::initializer_list<uint32_t> data_bytes) {
  if (data_bytes.size() <= 0 || data_bytes.size() > 6) {
    return std::unexpected(
      error::error::invalid_argument("data bytes must be between 1 and 6"));
  }

  std::array<uint32_t, 6> result{};
  std::copy_n(data_bytes.begin(), std::min(data_bytes.size(), size_t{6}),
              result.begin());

  unsigned int second_byte = 0;
  if (type == message_type::sysex_7bit_start) {
    second_byte = 0b00010000 | data_bytes.size();
  } else if (type == message_type::sysex_7bit_continue) {
    second_byte = 0b00100000 | data_bytes.size();
  } else if (type == message_type::sysex_7bit_end) {
    second_byte = 0b00110000 | data_bytes.size();
  }

  return {
    {
      (0b00110000 << 24 | second_byte << 16 | result[0] << 8 | result[1]),
      result[2] << 24 | result[3] << 16 | result[4] << 8 | result[5]
    }
  };
}
}
