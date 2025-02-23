#include "pmx/constants.h"

const std::array<std::string, 2> pmx::constants::service_dependencies = {
  "pipewire.service", "wireplumber.service"
};

const std::array<std::string, 7> pmx::constants::service_names = {
  "pmx-filter-chain-ctrl.service", "pmx-grpc-api.service",
  "pmx-metadata-manager.service", "pmx-midi-router.service",
  "pmx-osc-network-receiver.service", "pmx-osc-network-sender.service",
  "pmx-traktor-z1-router.service"
};

const std::array<std::string, 9> pmx::constants::all_services = {
  "pipewire.service", "wireplumber.service", "pmx-filter-chain-ctrl.service",
  "pmx-grpc-api.service", "pmx-metadata-manager.service",
  "pmx-midi-router.service", "pmx-osc-network-receiver.service",
  "pmx-osc-network-sender.service", "pmx-traktor-z1-router.service"
};

const std::array<std::string, 3> pmx::constants::pmx_osc_producer = {
  "pmx-midi-router.service", "pmx-osc-network-receiver.service",
  "pmx-traktor-z1-router.service"
};

const std::array<std::string, 2> pmx::constants::pmx_osc_consumer = {
  "pmx-filter-chain-ctrl.service", "pmx-osc-network-sender.service"
};
