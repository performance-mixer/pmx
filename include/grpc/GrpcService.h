#pragma once

#include "pmx_grpc.grpc.pb.h"

namespace grpc {

class GrpcService : public pmx::grpc::PmxGrpc::Service {
public:
  virtual ::grpc::Status ListPorts(::grpc::ServerContext *context,
                                   const ::pmx::grpc::ListPortsRequest *request,
                                   ::pmx::grpc::ListPortsResponse *response) {
    return ::grpc::Status::OK;
  }
};

} // namespace grpc
