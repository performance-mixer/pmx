#pragma once

#include "parameters/parameters.h"

#include <pwcpp/buffer.h>
#include <pwcpp/midi/control_change.h>

#include <spa/pod/builder.h>

#include <cstddef>
#include <string>

namespace processing {
std::string build_input_channel_osc_path(
  const pwcpp::midi::control_change &message,
  const parameters::parameter &parameter, const std::string &active_layer);

std::string build_group_channel_osc_path(uint8_t group_id,
                                         const parameters::parameter &parameter,
                                         const std::string &active_layer);

template <class Iter, size_t MAX_N>
void write_osc_messages(pwcpp::Buffer &buffer, spa_pod_builder &builder);
} // namespace processing
