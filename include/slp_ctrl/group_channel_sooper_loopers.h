#pragma once
#include <array>
#include <oscpp/server.hpp>
#include <pwcpp/buffer.h>

#include "sooper_looper.h"
#include "logging/logger.h"

namespace slp::ctrl {
class GroupChannelSooperLoopers {
public:
  GroupChannelSooperLoopers() {};

  void update_states(pwcpp::Buffer &buffer);

private:
  std::array<SooperLooper, 8> _loopers{
    SooperLooper(0, State::Off), SooperLooper(1, State::Off),
    SooperLooper(2, State::Off), SooperLooper(3, State::Off),
    SooperLooper(4, State::Off), SooperLooper(5, State::Off),
    SooperLooper(6, State::Off), SooperLooper(7, State::Off),
  };

  float _eighth_per_cycle = 0.0f;
  float _known_eighth_per_cycle = 0.0f;
  logging::Logger _logger{"group sooper loopers"};


  static State translate_state(const float state) {
    if (state == 0.0f) {
      return State::Off;
    }
    if (state == 1.0f) {
      return State::WaitStart;
    }
    if (state == 2.0f) {
      return State::Recording;
    }
    if (state == 3.0f) {
      return State::WaitStop;
    }
    if (state == 4.0f) {
      return State::Playing;
    }
    if (state == 5.0f) {
      return State::Overdubbing;
    }
    if (state == 6.0f) {
      return State::Multiplying;
    }
    if (state == 7.0f) {
      return State::Inserting;
    }
    if (state == 8.0f) {
      return State::Replacing;
    }
    if (state == 9.0f) {
      return State::Delay;
    }
    if (state == 10.0f) {
      return State::Muted;
    }
    if (state == 11.0f) {
      return State::Scratching;
    }
    if (state == 12.0f) {
      return State::OneShot;
    }
    if (state == 13.0f) {
      return State::Substitute;
    }
    if (state == 14.0f) {
      return State::Paused;
    }

    return State::Unknown;
  }
};
}
