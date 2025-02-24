#include "proxy/proxy_watcher.h"

#include <iostream>
#include <memory>
#include <spa/debug/pod.h>

void proxy::ProxyWatcher::register_callback(pw_registry *registry) {
  spa_zero(registry_hook);
  pw_registry_add_listener(registry, &registry_hook, &registry_events, this);
  this->registry = registry;
}

std::optional<pw_client*> proxy::ProxyWatcher::get_proxy_client(uint32_t id) {
  std::lock_guard lock(proxies_mutex);
  auto client = static_cast<pw_client*>(pw_registry_bind(
    registry, id, "PipeWire:Interface:Node", PW_VERSION_CLIENT, 0));
  if (client == nullptr) { return std::nullopt; }
  return client;
}

std::expected<void, pwcpp::error> proxy::ProxyWatcher::watch_props_param(
  std::string node_name) {
  std::lock_guard lock(watched_params_mutex);
  if (std::find(watched_props_params_nodes.begin(),
                watched_props_params_nodes.end(),
                node_name) == watched_props_params_nodes.end()) {
    watched_props_params_nodes.push_back(node_name);
    auto existing_node = std::find_if(proxies.begin(), proxies.end(),
                                      [node_name](const auto &proxy) {
                                        return proxy.name == node_name && proxy.
                                          type == proxy_type::node;
                                      });
    if (existing_node != proxies.end()) {
      std::uint32_t params[] = {SPA_PARAM_Props};
      if (!existing_node->connected) {
        pw_node_add_listener(existing_node->client().value(),
                             &existing_node->hook, &node_events, this);
        pw_node_subscribe_params(existing_node->client().value(), params, 1);
        existing_node->connected = true;
      }
    }
  }

  return {};
}

void proxy::ProxyWatcher::process_registry_event(void *data, uint32_t id,
                                                 uint32_t permissions,
                                                 const char *c_type,
                                                 uint32_t version,
                                                 const spa_dict *props) {
  auto *self = static_cast<ProxyWatcher*>(data);
  std::string_view type(c_type);
  if (type == "PipeWire:Interface:Node") {
    auto name = spa_dict_lookup(props, PW_KEY_NODE_NAME);
    std::lock_guard lock(self->proxies_mutex);
    auto factory = [self, id]() { return self->get_proxy_client(id); };
    self->proxies.push_back(Proxy(factory, proxy_type::node, id,
                                  std::string(name)));
    std::lock_guard watched_lock(self->watched_params_mutex);
    std::cout << "Checking if param is watched" << std::endl;
    if (std::find(self->watched_props_params_nodes.begin(),
                  self->watched_props_params_nodes.end(),
                  name) != self->watched_props_params_nodes.end()) {
      std::cout << "Param is watched" << std::endl;
      auto client = self->proxies.back().client();
      if (client.has_value()) {
        std::cout << "Adding listener" << std::endl;
        pw_node_add_listener(client.value(), &self->proxies.back().hook,
                             &self->node_events, self);
        std::uint32_t params[] = {SPA_PARAM_Props};
        pw_node_subscribe_params(client.value(), params, 1);
        std::cout << "Subscribed" << std::endl;
      }
    } else {
      std::cout << "Param not watched" << std::endl;
    }
  }
};

void proxy::ProxyWatcher::process_node_params_event(void *data, int seq,
                                                    std::uint32_t id,
                                                    uint32_t index,
                                                    uint32_t next,
                                                    const spa_pod *param) {
  auto *self = static_cast<ProxyWatcher*>(data);
  spa_debug_pod(0, nullptr, param);
}

std::optional<proxy::Proxy> proxy::ProxyWatcher::get_proxy(uint32_t id) {
  auto proxy = std::find_if(proxies.begin(), proxies.end(),
                            [id](const auto &proxy) {
                              return proxy.id == id;
                            });
  if (proxy != proxies.end()) {
    return *proxy;
  }
  return std::nullopt;
}
