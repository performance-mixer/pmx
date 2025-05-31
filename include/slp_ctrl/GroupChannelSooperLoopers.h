#pragma once
#include <array>
#include <oscpp/server.hpp>
#include <pwcpp/buffer.h>

#include "SooperLooper.h"

namespace slp::ctrl {
class GroupChannelSooperLoopers {
public:
  GroupChannelSooperLoopers();

  void update_states(pwcpp::Buffer &buffer) {
    const auto pod = buffer.get_pod(0);

    if (!pod.has_value()) {
      return;
    }

    if (!spa_pod_is_sequence(pod.value())) {
      return;
    }

    const auto sequence = reinterpret_cast<struct spa_pod_sequence*>(pod.
      value());
    spa_pod_control *pod_control;
    SPA_POD_SEQUENCE_FOREACH(sequence, pod_control) {
      if (pod_control->type != SPA_CONTROL_OSC) {
        std::cout << "not an osc message" << std::endl;
        continue;
      }

      uint8_t *data = nullptr;
      uint32_t length;
      spa_pod_get_bytes(&pod_control->value, (const void**)&data, &length);
      auto packet = OSCPP::Server::Packet(data, length);
      OSCPP::Server::Message message(packet);
      OSCPP::Server::ArgStream arguments_stream(message.args());

      std::string address(message.address());

      if (address == "/sl/global") {
        auto huh = arguments_stream.int32();
        auto parameter_name = std::string(arguments_stream.string());
        auto value = arguments_stream.float32();
        if (parameter_name == "eighth_per_cycle") {
          if (value != _eighth_per_cycle) {
            _logger.log_warning(
              "eight per cycle count changed to " + std::to_string(value));
          }
          _known_eighth_per_cycle = value;
        }
      } else if (address == "/sl/loops") {
        auto loop_index = arguments_stream.int32();
        auto parameter_name = std::string(arguments_stream.string());
        auto value = arguments_stream.float32();
        std::cout << "Received OSC message: " << value << std::endl;
        if (parameter_name == "state") {
          _loopers[loop_index].set_state(translate_state(value));
        }
      }
    }
  }

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
