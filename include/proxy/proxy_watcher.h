#pragma once

#include <boost/asio/detail/mutex.hpp>
#include <pipewire/pipewire.h>

namespace proxy {
enum class proxy_type { node };

class Proxy {
public:
  proxy_type type;
  uint32_t id;
  pw_client *client = nullptr;
};

class ProxyWatcher {
public:
  void register_callback(pw_registry *registry) {
    spa_zero(registry_hook);
    pw_registry_add_listener(registry, &registry_hook, &registry_events, this);
    this->registry = registry;
  }

  std::optional<pw_client*> get_proxy_client(uint32_t id) {
    auto proxy_iterator = std::find_if(proxies.begin(), proxies.end(),
                                       [id](const auto &proxy) {
                                         return proxy.id == id;
                                       });
    if (proxy_iterator != proxies.end() && proxy_iterator->client != nullptr) {
      return proxy_iterator->client;
    }

    if (proxy_iterator != proxies.end() && proxy_iterator->client == nullptr &&
      registry != nullptr) {
      proxy_iterator->client = static_cast<pw_client*>(pw_registry_bind(
        registry, id, "PipeWire:Interface:Node", PW_VERSION_CLIENT, 0));
      return proxy_iterator->client;
    }

    return {};
  }

private:
  pw_registry_events registry_events{
    .version = PW_VERSION_REGISTRY_EVENTS, .global = process_registry_event,
  };

  std::mutex proxies_mutex;
  std::vector<Proxy> proxies;
  pw_registry *registry{nullptr};

  spa_hook registry_hook{};

  static void process_registry_event(void *data, uint32_t id,
                                     uint32_t permissions, const char *c_type,
                                     uint32_t version, const spa_dict *props) {
    auto *self = static_cast<ProxyWatcher*>(data);
    std::string_view type(c_type);
    if (type == "PipeWire:Interface:Node") {
      std::lock_guard lock(self->proxies_mutex);
      self->proxies.push_back({proxy_type::node, id, nullptr});
    }
  };
};
}
