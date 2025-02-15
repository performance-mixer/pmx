#include "wpcpp/port_collection.h"

#include <pipewire/keys.h>

auto port_added_callback = +[ ](WpObjectManager *object_manager,
                                gpointer object, gpointer user_data) {
  auto *port_collection = static_cast<wpcpp::PortCollection*>(user_data);
  const auto g_object = static_cast<WpPipewireObject*>(object);

  const auto id = std::atoi(
    wp_pipewire_object_get_property(g_object, PW_KEY_PORT_ID));
  const auto name = std::string(
    wp_pipewire_object_get_property(g_object, PW_KEY_PORT_NAME));
  const auto alias = std::string(wp_pipewire_object_get_property(
    g_object, PW_KEY_PORT_ALIAS));
  const auto group = std::string(
    wp_pipewire_object_get_property(g_object, PW_KEY_PORT_GROUP));
  const auto path = std::string(
    wp_pipewire_object_get_property(g_object, PW_KEY_OBJECT_PATH));
  const auto dsp_format_unprocessed = wp_pipewire_object_get_property(
    g_object, PW_KEY_FORMAT_DSP);
  std::string dsp_format("unknown");
  if (dsp_format_unprocessed != nullptr) {
    dsp_format = std::string(dsp_format_unprocessed);
  }
  const auto node_id = std::atoi(
    wp_pipewire_object_get_property(g_object, PW_KEY_NODE_ID));
  const auto pw_direction = std::string(
    wp_pipewire_object_get_property(g_object, PW_KEY_PORT_DIRECTION));
  const auto physical = std::string(wp_pipewire_object_get_property(
    g_object, PW_KEY_PORT_PHYSICAL)) == "true";
  const auto object_serial = std::atoi(
    wp_pipewire_object_get_property(g_object, PW_KEY_OBJECT_SERIAL));

  wpcpp::pipewire_port::Direction direction;
  if (pw_direction ==
    "in") { direction = wpcpp::pipewire_port::Direction::IN; } else {
    direction = wpcpp::pipewire_port::Direction::OUT;
  }

  const auto is_monitor_string = wp_pipewire_object_get_property(
    g_object, PW_KEY_PORT_MONITOR);
  bool is_monitor(false);
  if (is_monitor_string != nullptr) {
    is_monitor = std::string(is_monitor_string) == "true";
  }

  port_collection->push_back(wpcpp::pipewire_port{
    id, name, alias, group, path, dsp_format, node_id, direction, physical,
    object_serial, is_monitor
  });
};

void wpcpp::PortCollection::setup(WpObjectManager *object_manager) {
  g_signal_connect(object_manager, "object-added",
                   G_CALLBACK(+port_added_callback), this);
}
