#pragma once

#include "parameters/parameters.h"

#include <pwcpp/buffer.h>
#include <pwcpp/midi/control_change.h>

#include <spa/pod/builder.h>

#include <cstddef>
#include <string>

namespace processing {

std::string build_input_channel_osc_path(const pwcpp::midi::control_change &message,
                                         const parameters::parameter &parameter);

template <class Iter, size_t MAX_N>
void write_osc_messages(pwcpp::Buffer &buffer, spa_pod_builder &builder);

} // namespace processing
