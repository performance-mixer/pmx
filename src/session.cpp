#include "slp_ctrl/session.h"

#include "ump/control_change.h"

#include <spa/control/control.h>

#include "ump/note.h"

void slp::ctrl::Session::add_launchpad_updates_to_sequence(
  spa_pod_builder &builder) const {
  for (auto &button : _buttons) {
    button.add_launchpad_updates_to_sequence(builder);
  }
}

void slp::ctrl::Button::add_launchpad_updates_to_sequence(
  spa_pod_builder &builder) const {
  if (_midi_type == MidiType::control_change) {
    add_control_change_update_to_sequence(builder);
  }

  if (_midi_type == MidiType::note) {
    add_note_update_to_sequence(builder);
  }
}

void slp::ctrl::Button::add_control_change_update_to_sequence(
  spa_pod_builder &builder) const {
  const auto channel_and_color = determine_channel_and_color();
  const auto message = ump::control_change_message(
    0, std::get<0>(channel_and_color), _midi_index,
    std::get<1>(channel_and_color));

  spa_pod_builder_control(&builder, 0, SPA_CONTROL_UMP);
  spa_pod_builder_bytes(&builder, &message, 4);
}

void slp::ctrl::Button::add_note_update_to_sequence(
  spa_pod_builder &builder) const {
  const auto channel_and_color = determine_channel_and_color();
  const auto message = ump::note_on_message(0, std::get<0>(channel_and_color),
                                            _midi_index,
                                            std::get<1>(channel_and_color));
  spa_pod_builder_control(&builder, 0, SPA_CONTROL_UMP);
  spa_pod_builder_bytes(&builder, &message, 4);
}

std::tuple<unsigned int, unsigned int>
slp::ctrl::Button::determine_channel_and_color() const {
  unsigned int color{_color.code};
  unsigned int channel{0};
  switch (_led_state) {
  case LedState::off:
    channel = 0;
    color = colors::black.code;
    break;
  case LedState::on:
    channel = 0;
    break;
  case LedState::flashing:
    channel = 1;
    break;
  case LedState::pulsing:
    channel = 2;
    break;
  }

  return {channel, color};
}
