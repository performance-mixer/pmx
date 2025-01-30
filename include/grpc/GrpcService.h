#pragma once

#include "pmx_grpc.grpc.pb.h"
#include "wpcpp/port.h"

#include <grpcpp/support/status.h>
#include <wpcpp/port_collection.h>

namespace grpc {
class GrpcService final : public pmx::grpc::PmxGrpc::Service {
public:
  explicit GrpcService(wpcpp::PortCollection &collection): _port_collection(
    collection) {}

  Status ListPorts(ServerContext *context,
                   const pmx::grpc::ListPortsRequest *request,
                   pmx::grpc::ListPortsResponse *response) override {
    auto ports = _port_collection.get_ports();
    for (auto &port : ports) {
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
      } else {
        direction = pmx::grpc::UNKNOWN;
      }

      response_port->set_direction(direction);
      response_port->set_physical(port.physical);
      response_port->set_alias(port.alias);
      response_port->set_group(port.group);
      response_port->set_path(port.path);
      response_port->set_dsp_format(port.dsp_format);
      response_port->set_audio_channel(port.audio_channel);
    }
    return Status::OK;
  }

private:
  wpcpp::PortCollection &_port_collection;
};
} // namespace grpc
