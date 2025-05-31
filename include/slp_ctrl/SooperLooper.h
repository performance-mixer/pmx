#pragma once

namespace slp::ctrl {
enum class State {
  Off,
  WaitStart,
  Recording,
  WaitStop,
  Playing,
  Overdubbing,
  Multiplying,
  Inserting,
  Replacing,
  Delay,
  Muted,
  Scratching,
  OneShot,
  Substitute,
  Paused,
  Unknown,
};

class SooperLooper {
public:
  SooperLooper(const size_t loop_index, const State state):
    _loop_index(loop_index), _state(state) {}

  const size_t &loop_index() const { return _loop_index; }
  const State &state() const { return _state; }

  void set_state(const State &state) { _state = state; }

private:
  size_t _loop_index;
  State _state;
};
}
