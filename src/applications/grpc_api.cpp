#include "grpc/GrpcService.h"
#include "wpcpp/port_collection.h"
#include "wpcpp/metadata.h"

#include <systemd/sd-daemon.h>

#include <thread>

#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <logging/logger.h>
#include <pipewire/keys.h>

#include <cxxopts/cxxopts.hpp>

#include <wp/wp.h>

struct WirePlumberControl {
  GOptionContext *context = nullptr;
  GMainLoop *loop = nullptr;
  WpCore *core = nullptr;
  WpObjectManager *om = nullptr;
};

int main(int argc, char **argv) {
  sd_notify(0, "STATUS=Starting");
  logging::Logger logger{"main"};

  cxxopts::Options options("pmx-grpc-api", "A gRPC API for pipewire");
  // @formatter:off
  options.add_options()
    ("h,help", "Print help")
    ("p,port", "Port to listen on",
     cxxopts::value<int>()->default_value("50051"));
  // @formatter:on

  auto result = options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    return 0;
  }

  int port = result["port"].as<int>();

  logger.log_info("Initializing wire plumber");
  WirePlumberControl wire_plumber_control;
  wp_init(WP_INIT_ALL);

  wire_plumber_control.context = g_option_context_new("pmx-grpc-api");
  wire_plumber_control.loop = g_main_loop_new(nullptr, false);
  wire_plumber_control.core = wp_core_new(nullptr, nullptr, nullptr);
  if (wp_core_connect(wire_plumber_control.core)) {
    logger.log_info("Installing object manager");
    wire_plumber_control.om = wp_object_manager_new();
    wp_object_manager_add_interest(wire_plumber_control.om, WP_TYPE_PORT,
                                   nullptr);
    wp_object_manager_request_object_features(wire_plumber_control.om,
                                              WP_TYPE_GLOBAL_PROXY,
                                              WP_PIPEWIRE_OBJECT_FEATURES_MINIMAL);

    wp_core_install_object_manager(wire_plumber_control.core,
                                   wire_plumber_control.om);

    logger.log_info("Creating port collection");
    wpcpp::PortCollection port_collection;
    port_collection.setup(wire_plumber_control.om);

    logger.log_info("Creating metadata collection");
    wpcpp::Metadata metadata;

    logger.log_info("Starting grpc service thread");
    std::thread grpc_service_thread([&port_collection, &metadata, &port]() {
      const auto address = std::string("0.0.0.0:") + std::to_string(port);
      logging::Logger logger{"grpc_service_thread"};
      grpc::GrpcService service(port_collection, metadata);
      grpc::ServerBuilder builder;
      builder.AddListeningPort(address, grpc::InsecureServerCredentials()).
              RegisterService(&service);
      const std::unique_ptr server(builder.BuildAndStart());
      logger.log_info("Server listening on " + address, "PORT=",
                      std::to_string(port));
      server->Wait();
    });

    logger.log_info("Setup metadata");
    metadata.get_existing_metadata_object(wire_plumber_control.core);

    sd_notify(0, "READY=1");

    logger.log_info("Starting main loop");
    g_main_loop_run(wire_plumber_control.loop);

    grpc_service_thread.join();
  }

  return 0;
}
