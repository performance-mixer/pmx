#include "wpcpp/proxy_collection.h"

#include <pipewire/keys.h>

#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>

std::string safe_string(const char *str) {
  if (str == nullptr) {
    return "";
  }
  return {str};
}

auto proxy_added_callback = +[ ](WpObjectManager *object_manager,
                                 gpointer object, gpointer user_data) {
  auto *proxy_collection = static_cast<wpcpp::ProxyCollection*>(user_data);
  const auto g_object = static_cast<WpPipewireObject*>(object);

  const auto object_id = std::atoi(wp_pipewire_object_get_property(
    g_object, PW_KEY_OBJECT_ID));

  std::string path = safe_string(
    wp_pipewire_object_get_property(g_object, PW_KEY_OBJECT_PATH));

  wpcpp::proxy_type type(wpcpp::proxy_type::unknown);
  std::string name;

  auto is_node = G_TYPE_CHECK_INSTANCE_TYPE(g_object, WP_TYPE_NODE);
  if (is_node) {
    name = safe_string(
      wp_pipewire_object_get_property(g_object, PW_KEY_NODE_NAME));
    type = wpcpp::proxy_type::node;
  }

  std::optional<int> node_id;
  auto is_port = G_TYPE_CHECK_INSTANCE_TYPE(g_object, WP_TYPE_PORT);
  if (is_port) {
    name = safe_string(
      wp_pipewire_object_get_property(g_object, PW_KEY_PORT_NAME));
    type = wpcpp::proxy_type::port;
    node_id = std::stoi(
      wp_pipewire_object_get_property(g_object, PW_KEY_NODE_ID));
  }

  proxy_collection->push_back({object_id, name, path, type, node_id});
};

void wpcpp::ProxyCollection::setup() {
  g_signal_connect(_object_manager, "object-added",
                   G_CALLBACK(+proxy_added_callback), this);
}
