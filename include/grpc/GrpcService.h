#pragma once

#include "pmx_grpc.grpc.pb.h"
#include "wpcpp/port.h"

#include <grpcpp/support/status.h>
#include <wpcpp/metadata_collection.h>
#include <wpcpp/port_collection.h>

namespace grpc {
inline const char *CHANNEL_PREFIX = "channel_";

class GrpcService final : public pmx::grpc::PmxGrpc::Service {
public:
  GrpcService(wpcpp::PortCollection &port_collection,
              wpcpp::MetadataCollection &metadata) : _port_collection(
                                                       port_collection),
                                                     _metadata(metadata) {}

  Status SetupInputPort(grpc::ServerContext *context,
                        const pmx::grpc::SetupInputPortRequest *request,
                        pmx::grpc::Response *response) override {
    std::ostringstream metadata_key_ss;
    metadata_key_ss << CHANNEL_PREFIX << request->channel_id();
    _metadata.set_metadata_value(metadata_key_ss.str(), request->port());

    response->set_success(true);
    return Status::OK;
  }

  Status ClearInputPort(ServerContext *context,
                        const pmx::grpc::ClearInputPortRequest *request,
                        pmx::grpc::Response *response) override {
    std::ostringstream metadata_key_ss;
    metadata_key_ss << CHANNEL_PREFIX << request->channel_id();
    _metadata.clear_metadata_value(metadata_key_ss.str());

    response->set_success(true);
    return Status::OK;
  }

  Status ListInputPortsSetup(ServerContext *context,
                             const pmx::grpc::EmptyRequest *request,
                             pmx::grpc::ListInputPortSetupResponse *
                             response) override {
    auto metadata = _metadata.get_metadata_values();
    std::sort(metadata.begin(), metadata.end(),
              [](const auto &a, const auto &b) {
                return std::stoi(std::get<0>(a).substr(8)) < std::stoi(
                  std::get<0>(b).substr(8));
              });

    for (auto &metadata_value : metadata) {
      if (std::get<0>(metadata_value).starts_with(CHANNEL_PREFIX)) {
        auto response_setup = response->add_setups();
        auto id_string = std::get<0>(metadata_value).substr(8);
        auto id = std::stoi(id_string);
        response_setup->set_channel_id(id);
        response_setup->set_port(std::get<1>(metadata_value));
      }
    }

    return Status::OK;
  }

  Status ListPorts(ServerContext *context,
                   const pmx::grpc::ListPortsRequest *request,
                   pmx::grpc::ListPortsResponse *response) override {
    for (auto ports = _port_collection.get_ports(); auto &port : ports) {
      auto response_port = response->add_ports();
      response_port->set_id(port.id);
      response_port->set_node_id(port.node_id);
      response_port->set_object_serial(port.object_serial);
      response_port->set_name(port.name);

      pmx::grpc::PortDirection direction;
      if (port.direction == wpcpp::pipewire_port::Direction::IN) {
        direction = pmx::grpc::PortDirection::IN;
      } else if (port.direction == wpcpp::pipewire_port::Direction::OUT) {
        direction = pmx::grpc::PortDirection::OUT;
      } else { direction = pmx::grpc::UNKNOWN; }

      response_port->set_direction(direction);
      response_port->set_physical(port.physical);
      response_port->set_alias(port.alias);
      response_port->set_group(port.group);
      response_port->set_path(port.path);
      response_port->set_dsp_format(port.dsp_format);
      response_port->set_is_monitor(port.is_monitor);
    }

    return Status::OK;
  }

private:
  wpcpp::PortCollection &_port_collection;
  wpcpp::MetadataCollection &_metadata;
};
} // namespace grpc
