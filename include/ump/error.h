#pragma once
#include <string>

namespace ump {
enum class error_type { UNKNOWN_MIDI_MESSAGE, MISSING_SECOND_VALUE, NOT_IMPLEMENTED };

struct error {
  std::string message;
  error_type type;

  static error unknown_midi_message(const std::string &message) {
    return {message, error_type::UNKNOWN_MIDI_MESSAGE};
  }

  static error missing_second_value() {
    return {"Missing second value", error_type::MISSING_SECOND_VALUE};
  }

  static error not_implemented() {
    return {"Not implemented", error_type::NOT_IMPLEMENTED};
  }
};
}
