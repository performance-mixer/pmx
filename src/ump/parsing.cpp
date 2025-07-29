#include "ump/parsing.h"

std::expected<ump::messages::message, ump::error> ump::parse(uint32_t first,
  std::optional<uint32_t> second) {
  auto message_type = first >> 28;
  if (message_type == 2) {
    const auto group = static_cast<std::uint8_t>(first >> 24 & 0b00001111);
    const auto channel = static_cast<std::uint8_t>(first >> 16 & 0b00001111);
    const auto note_number = static_cast<std::uint8_t>(first >> 8 & 0b01111111);
    const auto velocity = static_cast<std::uint8_t>(first & 0b01111111);
    const auto channel_voice_message_type = first >> 20 & 0b0001111;

    if (channel_voice_message_type == 0b01000) {
      return messages::v1::note_off{group, channel, note_number, velocity};
    } else if (channel_voice_message_type == 0b01001) {
      return messages::v1::note_on{group, channel, note_number, velocity};
    } else if (channel_voice_message_type == 0b01011) {
      return messages::v1::control_change{
        group, channel, note_number, velocity
      };
    }
  } else if (message_type == 4) {
    if (second.has_value()) {
      const auto channel_voice_message_type = first >> 20 & 0b0001111;
      if (channel_voice_message_type == 0b01000 || channel_voice_message_type ==
        0b01001) {
        const auto group = static_cast<std::uint8_t>(first >> 24 & 0b00001111);
        const auto channel = static_cast<std::uint8_t>(first >> 16 &
          0b00001111);
        const auto note_number = static_cast<std::uint8_t>(first >> 8 &
          0b01111111);
        const auto msg_attribute_type = static_cast<std::uint8_t>(first &
          0b11111111);
        auto attribute_type = ump::messages::v2::attribute_type::none;
        if (msg_attribute_type == 1) {
          attribute_type = messages::v2::attribute_type::manufacturer;
        } else if (msg_attribute_type == 2) {
          attribute_type = messages::v2::attribute_type::profile;
        } else if (msg_attribute_type == 3) {
          attribute_type = messages::v2::attribute_type::pitch;
        }

        const auto velocity = second.value() >> 16;
        const auto value = second.value() & 0b1111111111111111;

        if (channel_voice_message_type == 0b1000) {
          return messages::v2::note_off(group, channel, note_number, velocity,
                                        attribute_type, value);
        }

        if (channel_voice_message_type == 0b1001) {
          return messages::v2::note_on(group, channel, note_number, velocity,
                                       attribute_type, value);
        }
      } else if (channel_voice_message_type == 0b01011) {
        const auto group = static_cast<std::uint8_t>(first >> 24 & 0b00001111);
        const auto channel = static_cast<std::uint8_t>(first >> 16 &
          0b00001111);
        const auto index = static_cast<std::uint8_t>(first >> 8 & 0b01111111);
        return messages::v2::control_change(group, channel, index,
                                            second.value());
      }
    }

    return std::unexpected(error::missing_second_value());
  }

  return std::unexpected(error::unknown_midi_message("unknown midi message"));
}
