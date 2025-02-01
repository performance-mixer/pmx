#pragma once

#include "pmx_grpc.grpc.pb.h"
#include "wpcpp/port.h"

#include <grpcpp/support/status.h>
#include <wpcpp/metadata_collection.h>
#include <wpcpp/port_collection.h>

namespace grpc {
class GrpcService final : public pmx::grpc::PmxGrpc::Service {
public:
  GrpcService(wpcpp::PortCollection &port_collection,
              wpcpp::MetadataCollection &metadata) : _port_collection(
      port_collection),
    _metadata(metadata) {}

  Status SetOutputPorts(ServerContext * context,
                        const pmx::grpc::SetOutputPortsRequest * request,
                        pmx::grpc::Response * response) override {
    std::ostringstream left_ss;
    std::ostringstream right_ss;
    left_ss << "[";
    right_ss << "[";
    for (auto &&port : request->ports()) {
      left_ss << "'" << port.left() << "', ";
      right_ss << "'" << port.right() << "', ";
    }

    if (left_ss.str().size() > 2) {
      left_ss.seekp(-2, std::ios_base::end);
      left_ss << "]";
    }

    if (right_ss.str().size() > 2) {
      right_ss.seekp(-2, std::ios_base::end);
      right_ss << "]";
    }

    _metadata.set_metadata_value("left_outputs", left_ss.str());
    _metadata.set_metadata_value("right_outputs", right_ss.str());
    response->set_success(true);
    return Status::OK;
  };

  Status SetupInputPort(grpc::ServerContext * context,
                        const pmx::grpc::SetupInputPortRequest * request,
                        pmx::grpc::Response * response) override {
    std::ostringstream metadata_key_ss;
    metadata_key_ss << "channel_" << request->channel_id();
    _metadata.set_metadata_value(metadata_key_ss.str(), request->port());

    response->set_success(true);
    return Status::OK;
  }

  Status ClearInputPort(ServerContext * context,
                        const pmx::grpc::ClearInputPortRequest * request,
                        pmx::grpc::Response * response) override {
    std::ostringstream metadata_key_ss;
    metadata_key_ss << "channel_" << request->channel_id();
    _metadata.clear_metadata_value(metadata_key_ss.str());

    response->set_success(true);
    return Status::OK;
  }

  Status ListPorts(ServerContext * context,
                   const pmx::grpc::ListPortsRequest * request,
                   pmx::grpc::ListPortsResponse * response) override {
    for (auto ports = _port_collection.get_ports(); auto &port : ports) {
      auto response_port = response->add_ports();
      response_port->set_id(port.id);
      response_port->set_node_id(port.node_id);
      response_port->set_object_serial(port.object_serial);
      response_port->set_name(port.name);

      pmx::grpc::PortDirection direction;
      if (port.direction == wpcpp::pipewire_port::Direction::IN) {
        direction = pmx::grpc::PortDirection::IN;
      }
      else if (port.direction == wpcpp::pipewire_port::Direction::OUT) {
        direction = pmx::grpc::PortDirection::OUT;
      }
      else { direction = pmx::grpc::UNKNOWN; }

      response_port->set_direction(direction);
      response_port->set_physical(port.physical);
      response_port->set_alias(port.alias);
      response_port->set_group(port.group);
      response_port->set_path(port.path);
      response_port->set_dsp_format(port.dsp_format);
    }

    return Status::OK;
  }

private:
  wpcpp::PortCollection &_port_collection;
  wpcpp::MetadataCollection &_metadata;
};

} // namespace grpc
