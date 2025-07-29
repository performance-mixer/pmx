#pragma once
#include <cstdint>
#include <variant>

namespace ump::messages::v1 {
struct note_on {
  std::uint8_t group;
  std::uint8_t channel;
  std::uint8_t note_number;
  std::uint8_t velocity;
};

struct note_off {
  std::uint8_t group;
  std::uint8_t channel;
  std::uint8_t note_number;
  std::uint8_t velocity;
};

struct control_change {
  std::uint8_t group;
  std::uint8_t channel;
  std::uint8_t index;
  std::uint8_t value;
};
}

namespace ump::messages::v2 {
enum class attribute_type {
  none,
  manufacturer,
  profile,
  pitch
};

struct note_on {
  std::uint8_t group;
  std::uint8_t channel;
  std::uint8_t note_number;
  std::uint16_t velocity;
  enum ump::messages::v2::attribute_type attribute_type;
  std::uint16_t attribute_value;
};

struct note_off {
  std::uint8_t group;
  std::uint8_t channel;
  std::uint8_t note_number;
  std::uint16_t velocity;
  enum ump::messages::v2::attribute_type attribute_type;
  std::uint16_t attribute_value;
};

struct control_change {
  std::uint8_t group;
  std::uint8_t channel;
  std::uint8_t index;
  std::uint32_t value;
};
}

namespace ump::messages {
using message = std::variant<v1::note_on, v1::note_off, v1::control_change,
                             v2::note_on, v2::note_off, v2::control_change>;
}
