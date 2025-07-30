#pragma once

#include <variant>

namespace slp::ctrl::launchpad::mini::session {
struct button_down {
  unsigned int row;
  unsigned int column;
};

struct button_up {
  unsigned int row;
  unsigned int column;
};

struct fader_value_change {
  unsigned int index;
  unsigned int value;
};

using event = std::variant<button_down, button_up, fader_value_change>;
}
