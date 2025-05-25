#pragma once

#include <array>
#include <string>

namespace pmx::constants {
extern const std::array<std::string, 2> service_dependencies;
extern const std::array<std::string, 13> service_names;
extern const std::array<std::string, 16> all_services;
extern const std::array<std::string, 3> pmx_osc_producer_node_names;
extern const std::array<std::string, 2> pmx_osc_consumer_node_names;
extern const std::array<std::string, 8> filter_chain_node_names;
extern const std::array<std::string, 8> filter_chain_in_node_names;
extern const std::array<std::string, 8> filter_chain_out_node_names;
}
