#pragma once

#include "pmx_grpc.grpc.pb.h"
#include "wpcpp/port.h"
#include "config/prefix.h"

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

  Status SetupInputPort(ServerContext *context,
                        const pmx::grpc::SetupInputPortRequest *request,
                        pmx::grpc::Response *response) override {
    if (request->has_port()) {
      std::ostringstream metadata_key_ss;
      metadata_key_ss << config::Prefix::CHANNEL_PORT_PREFIX << request->
        channel_id();
      _metadata.set_metadata_value(metadata_key_ss.str(), request->port());
    }

    if (request->has_group_channel_id()) {
      std::ostringstream metadata_key_ss;
      metadata_key_ss << config::Prefix::CHANNEL_GROUP_PREFIX << request->
        channel_id();
      _metadata.set_metadata_value(metadata_key_ss.str(),
                                   std::to_string(request->group_channel_id()));
    }

    response->set_success(true);
    return Status::OK;
  }

  Status ClearInputPort(ServerContext *context,
                        const pmx::grpc::ClearInputPortRequest *request,
                        pmx::grpc::Response *response) override {
    std::ostringstream metadata_key_ss;
    metadata_key_ss << config::Prefix::CHANNEL_PORT_PREFIX << request->
      channel_id();
    _metadata.clear_metadata_value(metadata_key_ss.str());

    response->set_success(true);
    return Status::OK;
  }

  Status ListInputPortsSetup(ServerContext *context,
                             const pmx::grpc::EmptyRequest *request,
                             pmx::grpc::ListInputPortSetupResponse *
                             response) override {
    std::unordered_map<size_t, std::string> channel_id_to_port;
    std::unordered_map<size_t, size_t> channel_id_to_group_channel_id;
    auto metadata = _metadata.get_metadata_values();
    for (auto &&metadata_item : metadata) {
      if (std::get<0>(metadata_item).starts_with(
        config::Prefix::CHANNEL_PORT_PREFIX)) {
        auto channel_id = std::stoi(std::get<0>(metadata_item).substr(13));
        channel_id_to_port[channel_id] = std::get<1>(metadata_item);
      } else if (std::get<0>(metadata_item).starts_with(
        config::Prefix::CHANNEL_GROUP_PREFIX)) {
        auto channel_id = std::stoi(std::get<0>(metadata_item).substr(14));
        channel_id_to_group_channel_id[channel_id] = std::stoi(
          std::get<1>(metadata_item));
      }
    }

    std::unordered_set<size_t> union_keys;
    for (const auto &pair : channel_id_to_port) {
      union_keys.insert(pair.first);
    }

    for (const auto &pair : channel_id_to_group_channel_id) {
      union_keys.insert(pair.first);
    }

    for (const auto &key : union_keys) {
      auto response_setup = response->add_setups();
      response_setup->set_channel_id(key);

      if (channel_id_to_port.contains(key)) {
        response_setup->set_port(channel_id_to_port[key]);
      }

      if (channel_id_to_group_channel_id.contains(key)) {
        response_setup->set_group_channel_id(
          channel_id_to_group_channel_id[key]);
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
