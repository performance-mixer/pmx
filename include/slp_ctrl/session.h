#pragma once

#include <spa/pod/builder.h>

#include "launchpad_colors.h"

namespace slp::ctrl {
enum class LedState {
  off,
  on,
  flashing,
  pulsing
};

enum class ButtonType {
  top_row,
  rightmost_column,
  standard,
  logo
};

enum class MidiType { note, control_change };

class Button {
public:
  Button(LedState led_state, const launchpad_color &color,
         ButtonType button_type, MidiType midi_type, std::uint8_t midi_value)
    : _led_state(led_state), _color(color), _button_type(button_type),
      midi_type(midi_type), midi_value(midi_value) {}

private:
  bool _pressed{false};
  LedState _led_state{LedState::off};
  const launchpad_color &_color;
  ButtonType _button_type;
  MidiType midi_type;
  std::uint8_t midi_value;
};

class Session {
public:
  Button &button(size_t row, size_t column) {
    return _buttons[row * 9 + column];
  }

  const Button &button(size_t row, size_t column) const {
    return _buttons[row * 9 + column];
  }

  void add_launchpad_updates_to_builder(spa_pod_builder &builder) const;

private:
  std::array<Button, 81> _buttons{
    Button(LedState::off, colors::black, ButtonType::top_row,
           MidiType::control_change, 91),
    Button(LedState::off, colors::black, ButtonType::top_row,
           MidiType::control_change, 92),
    Button(LedState::off, colors::black, ButtonType::top_row,
           MidiType::control_change, 93),
    Button(LedState::off, colors::black, ButtonType::top_row,
           MidiType::control_change, 94),
    Button(LedState::off, colors::black, ButtonType::top_row,
           MidiType::control_change, 95),
    Button(LedState::off, colors::black, ButtonType::top_row,
           MidiType::control_change, 96),
    Button(LedState::off, colors::black, ButtonType::top_row,
           MidiType::control_change, 97),
    Button(LedState::off, colors::black, ButtonType::top_row,
           MidiType::control_change, 98),
    Button(LedState::pulsing, colors::purple, ButtonType::logo,
           MidiType::control_change, 99),
    Button(LedState::on, colors::amber, ButtonType::standard, MidiType::note,
           81),
    Button(LedState::on, colors::amber, ButtonType::standard, MidiType::note,
           82),
    Button(LedState::on, colors::amber, ButtonType::standard, MidiType::note,
           83),
    Button(LedState::on, colors::amber, ButtonType::standard, MidiType::note,
           84),
    Button(LedState::on, colors::amber, ButtonType::standard, MidiType::note,
           85),
    Button(LedState::on, colors::amber, ButtonType::standard, MidiType::note,
           86),
    Button(LedState::on, colors::amber, ButtonType::standard, MidiType::note,
           87),
    Button(LedState::on, colors::amber, ButtonType::standard, MidiType::note,
           88),
    Button(LedState::on, colors::purple, ButtonType::rightmost_column,
           MidiType::control_change, 89),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           71),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           72),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           73),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           74),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           75),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           76),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           77),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           78),
    Button(LedState::on, colors::aqua_blue, ButtonType::rightmost_column,
           MidiType::control_change, 79),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           61),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           62),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           63),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           64),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           65),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           66),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           67),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           68),
    Button(LedState::on, colors::aqua_blue, ButtonType::rightmost_column,
           MidiType::control_change, 69),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           51),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           52),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           53),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           54),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           55),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           56),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           57),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           58),
    Button(LedState::on, colors::aqua_blue, ButtonType::rightmost_column,
           MidiType::control_change, 59),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           41),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           42),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           43),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           44),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           45),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           46),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           47),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           48),
    Button(LedState::on, colors::aqua_blue, ButtonType::rightmost_column,
           MidiType::control_change, 49),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           31),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           32),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           33),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           34),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           35),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           36),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           37),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           38),
    Button(LedState::on, colors::aqua_blue, ButtonType::rightmost_column,
           MidiType::control_change, 39),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           21),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           22),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           23),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           24),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           25),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           26),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           27),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           28),
    Button(LedState::on, colors::aqua_blue, ButtonType::rightmost_column,
           MidiType::control_change, 29),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           11),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           12),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           13),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           14),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           15),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           16),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           17),
    Button(LedState::off, colors::black, ButtonType::standard, MidiType::note,
           18),
    Button(LedState::on, colors::aqua_blue, ButtonType::rightmost_column,
           MidiType::control_change, 19),
  };
};
}
