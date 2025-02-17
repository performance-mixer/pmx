#include "processing.h"

#include <pwcpp/midi/parse_midi.h>

#include <oscpp/client.hpp>

#include <spa/pod/builder.h>

#include <format>
#include <sstream>

std::string
processing::build_input_channel_osc_path(
  const pwcpp::midi::control_change &message,
  const parameters::parameter &parameter) {
  std::stringstream osc_path;

  osc_path << std::format("/I/A/{}/{}/{}", static_cast<int>(message.channel) + 1,
                          parameter.device_short_name, parameter.name);

  return osc_path.str();
}

std::string processing::build_group_channel_osc_path(uint8_t group_id,
                                                     const parameters::parameter
                                                     &parameter) {
  return std::format("/G/A/{}/{}/{}", group_id, parameter.device_short_name,
                     parameter.name);
};

template <class Iter, size_t MAX_N>
void processing::write_osc_messages(pwcpp::Buffer &buffer,
                                    spa_pod_builder &builder) {
  auto buffer_midi_messages = pwcpp::midi::parse_midi<MAX_N>(buffer);
  for (auto &&midi_message : buffer_midi_messages) {
    if (midi_message.has_value()) {
      auto control_change = get<pwcpp::midi::control_change>(
        midi_message.value());
      auto parameter = parameters::find_target_parameter_for_input_channels(
        control_change.cc_number);
      auto path = build_input_channel_osc_path(control_change, *parameter);

      char osc_buffer[4096];
      OSCPP::Client::Packet packet(osc_buffer, 4096);
      packet.openMessage(path.c_str(), 1).float32(control_change.value).
             closeMessage();
      const auto size = packet.size();
      spa_pod_builder_bytes(&builder, osc_buffer, size);
    }
  }
}
