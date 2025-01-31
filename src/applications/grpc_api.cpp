#include "grpc/GrpcService.h"
#include "wpcpp/port_collection.h"
#include "wpcpp/metadata_collection.h"

#include <thread>

#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <pipewire/keys.h>

#include <wp/wp.h>

struct WirePlumberControl {
  GOptionContext * context = nullptr;
  GMainLoop * loop = nullptr;
  WpCore * core = nullptr;
  WpObjectManager * om = nullptr;
};

int main(int argc, char ** argv) {
  WirePlumberControl wire_plumber_control;
  wp_init(WP_INIT_ALL);

  wire_plumber_control.context = g_option_context_new("pmx-grpc-api");
  wire_plumber_control.loop = g_main_loop_new(nullptr, false);
  wire_plumber_control.core = wp_core_new(nullptr, nullptr, nullptr);
  if (wp_core_connect(wire_plumber_control.core)) {
    wire_plumber_control.om = wp_object_manager_new();
    wp_object_manager_add_interest(wire_plumber_control.om, WP_TYPE_PORT,
                                   nullptr);
    wp_object_manager_request_object_features(wire_plumber_control.om,
                                              WP_TYPE_GLOBAL_PROXY,
                                              WP_PIPEWIRE_OBJECT_FEATURES_MINIMAL);

    wp_core_install_object_manager(wire_plumber_control.core,
                                   wire_plumber_control.om);

    wpcpp::PortCollection port_collection;
    port_collection.setup(wire_plumber_control.om);

    wpcpp::MetadataCollection metadata;

    std::thread grpc_service_thread([&port_collection, &metadata]() {
      grpc::GrpcService service(port_collection, metadata);
      grpc::ServerBuilder builder;
      builder.AddListeningPort("0.0.0.0:50051",
                               grpc::InsecureServerCredentials()).
              RegisterService(&service);
      const std::unique_ptr server(builder.BuildAndStart());
      server->Wait();
    });

    metadata.register_initial_metadata_object(wire_plumber_control.core);
    g_main_loop_run(wire_plumber_control.loop);

    grpc_service_thread.join();
  }

  return 0;
}
