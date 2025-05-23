#include "pmx/constants.h"

const std::array<std::string, 2> pmx::constants::service_dependencies = {
  "pipewire.service", "wireplumber.service"
};

const std::array<std::string, 12> pmx::constants::service_names = {
  "pmx-filter-chain-ctrl.service", "pmx-grpc-api.service",
  "pmx-metadata-manager.service", "pmx-midi-router.service",
  "pmx-osc-network-receiver.service", "pmx-osc-network-sender.service",
  "pmx-traktor-z1-router.service", "pmx-filter-chains.service",
  "pmx-jack-midi-clock.service", "pmx-osc-sl-network-receiver.service",
  "pmx-osc-sl-network-sender.service", "pmx-slp-ctrl.service",
};

const std::array<std::string, 15> pmx::constants::all_services = {
  "pipewire.service", "wireplumber.service", "pmx-filter-chain-ctrl.service",
  "pmx-grpc-api.service", "pmx-metadata-manager.service",
  "pmx-midi-router.service", "pmx-osc-network-receiver.service",
  "pmx-osc-network-sender.service", "pmx-traktor-z1-router.service",
  "pmx-filter-chains.service", "pmx-jack-midi-clock.service",
  "pmx-osc-network-receiver.service", "pmx-osc-network-sender.service",
  "pmx-sooperlooper.service", "pmx-slp-ctrl.service",
};

const std::array<std::string, 3> pmx::constants::pmx_osc_producer_node_names = {
  "pmx-midi-router", "pmx-osc-network-receiver", "pmx-traktor-z1-router"
};

const std::array<std::string, 2> pmx::constants::pmx_osc_consumer_node_names = {
  "pmx-filter-chain-ctrl", "pmx-osc-network-sender"
};

const std::array<std::string, 8> pmx::constants::filter_chain_node_names = {
  "pmx-layer-channels-ins", "pmx-layer-channels-outs",
  "pmx-group-channels-a-ins", "pmx-group-channels-b-ins",
  "pmx-group-channels-a-outs", "pmx-group-channels-b-outs",
  "pmx-input-channels-ins", "pmx-input-channels-outs",
};

const std::array<std::string, 8> pmx::constants::filter_chain_in_node_names = {
  "pmx-layer-channels-ins", "pmx-group-channels-a-ins",
  "pmx-group-channels-b-ins", "pmx-input-channels-ins",
};

const std::array<std::string, 8> pmx::constants::filter_chain_out_node_names = {
  "pmx-layer-channels-outs", "pmx-group-channels-a-outs",
  "pmx-group-channels-b-outs", "pmx-input-channels-outs",
};
