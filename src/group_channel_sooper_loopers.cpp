#include "slp_ctrl/group_channel_sooper_loopers.h"

void slp::ctrl::GroupChannelSooperLoopers::update_states(
  pwcpp::Buffer &buffer) {
  const auto pod = buffer.get_pod(0);

  if (!pod.has_value()) {
    return;
  }

  if (!spa_pod_is_sequence(pod.value())) {
    return;
  }

  const auto sequence = reinterpret_cast<struct spa_pod_sequence*>(pod.value());
  spa_pod_control *pod_control;
  SPA_POD_SEQUENCE_FOREACH(sequence, pod_control) {
    if (pod_control->type != SPA_CONTROL_OSC) {
      _logger.log_warning("not an osc message");
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
      if (parameter_name == "state") {
        _loopers[loop_index].set_state(translate_state(value));
      }
    }
  }
}
