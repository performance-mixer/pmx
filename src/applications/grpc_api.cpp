#include "grpc/GrpcService.h"
#include "wpcpp/port_collection.h"

#include <thread>

#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <pipewire/keys.h>

#include <wp/wp.h>

struct WirePlumberControl {
  GOptionContext *context = nullptr;
  GMainLoop *loop = nullptr;
  WpCore *core = nullptr;
  WpObjectManager *om = nullptr;
};


auto port_added_callback = +[ ](WpObjectManager *object_manager,
                                const gpointer object,
                                const gpointer user_data) {
  auto *port_collection = static_cast<wpcpp::PortCollection*>(user_data);
  const auto g_object = static_cast<WpPipewireObject*>(object);

  const auto alias = wp_pipewire_object_get_property(
    g_object, PW_KEY_PORT_ALIAS);
  port_collection->push_back(wpcpp::pipewire_port{.alias = std::string(alias)});
};

int main(int argc, char **argv) {
  WirePlumberControl wire_plumber_service;
  wp_init(WP_INIT_ALL);

  wire_plumber_service.context = g_option_context_new("pmx-grpc-api");
  wire_plumber_service.loop = g_main_loop_new(nullptr, false);
  wire_plumber_service.core = wp_core_new(nullptr, nullptr, nullptr);
  if (wp_core_connect(wire_plumber_service.core)) {
    wire_plumber_service.om = wp_object_manager_new();
    wp_object_manager_add_interest(wire_plumber_service.om, WP_TYPE_PORT,
                                   nullptr);
    wp_object_manager_request_object_features(wire_plumber_service.om,
                                              WP_TYPE_GLOBAL_PROXY,
                                              WP_PIPEWIRE_OBJECT_FEATURES_MINIMAL);

    wp_core_install_object_manager(wire_plumber_service.core,
                                   wire_plumber_service.om);

    wpcpp::PortCollection port_collection;
    g_signal_connect(wire_plumber_service.om, "object-added",
                     G_CALLBACK(+port_added_callback), &port_collection);

    std::thread grpc_service_thread([&port_collection]() {
      grpc::GrpcService service(port_collection);
      grpc::ServerBuilder builder;
      builder.AddListeningPort("0.0.0.0:50051",
                               grpc::InsecureServerCredentials()).
              RegisterService(&service);
      const std::unique_ptr server(builder.BuildAndStart());
      server->Wait();
    });

    g_main_loop_run(wire_plumber_service.loop);

    grpc_service_thread.join();
  }

  return 0;
}

