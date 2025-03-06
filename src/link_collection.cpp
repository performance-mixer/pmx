#include "wpcpp/link_collection.h"

#include <pipewire/keys.h>
#include <wp/link.h>
#include <wp/object-manager.h>
#include <wp/proxy-interfaces.h>

auto link_added_callback = +[ ](WpObjectManager *object_manager,
                                gpointer object, gpointer user_data) {
  const auto link_collection = static_cast<wpcpp::LinkCollection*>(user_data);
  const auto link = static_cast<WpPipewireObject*>(object);
  const auto id = std::stoi(
    wp_pipewire_object_get_property(link, PW_KEY_OBJECT_ID));

  const auto output_port_id = std::stoi(
    wp_pipewire_object_get_property(link, PW_KEY_LINK_OUTPUT_PORT));

  const auto input_port_id = std::stoi(
    wp_pipewire_object_get_property(link, PW_KEY_LINK_INPUT_PORT));

  const auto output_node_id = std::stoi(
    wp_pipewire_object_get_property(link, PW_KEY_LINK_OUTPUT_NODE));

  const auto input_node_id = std::stoi(
    wp_pipewire_object_get_property(link, PW_KEY_LINK_INPUT_NODE));

  link_collection->push_back({
    id, output_port_id, input_port_id, output_node_id, input_node_id
  });
};

std::shared_ptr<wpcpp::LinkCollection> wpcpp::LinkCollection::create_and_setup(
  WpCore *core) {
  auto object_manager = wp_object_manager_new();

  wp_object_manager_add_interest(object_manager, WP_TYPE_LINK, nullptr);

  wp_object_manager_request_object_features(object_manager,
                                            WP_TYPE_GLOBAL_PROXY,
                                            WP_PIPEWIRE_OBJECT_FEATURES_MINIMAL);

  wp_core_install_object_manager(core, object_manager);

  auto collection = std::make_shared<LinkCollection>(object_manager);
  g_signal_connect(object_manager, "object-added",
                   G_CALLBACK(+link_added_callback), collection.get());

  return collection;
};
