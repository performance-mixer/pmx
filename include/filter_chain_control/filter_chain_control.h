#pragma once

#include <osc/parse_osc_path.h>
#include <oscpp/server.hpp>
#include <tools/filter_parameter_tools.h>
#include <logging/logger.h>

namespace filter_chain_control {
template <typename TQueue, typename TParamCollection>
bool get_filter_chain_id_and_add_to_queue(logging::Logger &logger,
                                          TQueue &queue, int id,
                                          TParamCollection parameters,
                                          const osc::osc_path &osc_path,
                                          OSCPP::Server::Message &message) {
  auto filter_chain_id = tools::get_from_collection<int>(id, parameters);
  if (filter_chain_id.has_value()) {
    logger.log_info(std::format("Preparing request for {}, {}", id,
                                message.address()));
    OSCPP::Server::ArgStream arguments_stream(message.args());
    queue.push({osc_path, filter_chain_id.value(), arguments_stream.float32()});
    return true;
  } else {
    logger.log_warning(std::format("Parameter id {} no set", id));
    return false;
  }
}
}
