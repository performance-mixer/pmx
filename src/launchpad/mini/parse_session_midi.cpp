#include "slp_ctrl/launchpad/mini/parse_session_midi.h"

#include "ump/parsing.h"

bool note_number_is_invalid(const unsigned int note_number) {
  return note_number < 11 || note_number > 99;
}

std::expected<slp::ctrl::launchpad::mini::session::event, error::error>
slp::ctrl::launchpad::mini::parse_session_midi(const unsigned int first,
                                               const std::optional<unsigned int>
                                               second) {
  auto message = ump::parse(first, second);
  if (!message.has_value()) {
    const auto &error = message.error();
    return std::unexpected(error::error::ump_parsing_error(error.message));
  }

  const auto variant = message.value();

  if (std::holds_alternative<ump::messages::v2::note_on>(variant)) {
    const auto note_on = std::get<ump::messages::v2::note_on>(variant);
    if (note_number_is_invalid(note_on.note_number)) {
      return std::unexpected(
        error::error::invalid_note_number(note_on.note_number));
    }

    auto row = (100 - note_on.note_number) / 10;
    auto column = (note_on.note_number % 10) - 1;
    return session::button_down(row, column);
  } else if (std::holds_alternative<ump::messages::v2::note_off>(variant)) {
    const auto note_off = std::get<ump::messages::v2::note_off>(variant);
    if (note_number_is_invalid(note_off.note_number)) {
      return std::unexpected(
        error::error::invalid_note_number(note_off.note_number));
    }

    auto row = (100 - note_off.note_number) / 10;
    auto column = (note_off.note_number % 10) - 1;
    return session::button_up(row, column);
  } else if (std::holds_alternative<ump::messages::v1::note_on>(variant)) {
    const auto note_on = std::get<ump::messages::v1::note_on>(variant);
    if (note_number_is_invalid(note_on.note_number)) {
      return std::unexpected(
        error::error::invalid_note_number(note_on.note_number));
    }

    auto row = (100 - note_on.note_number) / 10;
    auto column = (note_on.note_number % 10) - 1;
    return session::button_down(row, column);
  } else if (std::holds_alternative<ump::messages::v1::note_off>(variant)) {
    const auto note_off = std::get<ump::messages::v1::note_off>(variant);
    if (note_number_is_invalid(note_off.note_number)) {
      return std::unexpected(
        error::error::invalid_note_number(note_off.note_number));
    }

    auto row = (100 - note_off.note_number) / 10;
    auto column = (note_off.note_number % 10) - 1;
    return session::button_up(row, column);
  }

  return std::unexpected(error::error::not_implemented());
}
