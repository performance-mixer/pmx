#pragma once

#include <expected>
#include <boost/asio/detail/mutex.hpp>
#include <pipewire/pipewire.h>
#include <pwcpp/error.h>

namespace proxy {
struct param {
  std::string name;
  std::string value;
  std::string type;
};

struct node_props_params {
  int node_id;
  std::vector<param> params;
};

enum class proxy_type { node };

class Proxy {
public:
  Proxy(std::function<std::optional<pw_client*>()> client_factory,
        proxy_type type, uint32_t id, std::string name) : type(type), id(id),
                                                          name(name),
                                                          _client_factory(
                                                            client_factory) {}

  std::optional<pw_client*> client() {
    if (!_client.has_value()) {
      _client = _client_factory();
    }
    return _client;
  };

  proxy_type type;
  uint32_t id;
  std::string name;
  bool connected = false;
  spa_hook hook{};

private:
  std::function<std::optional<pw_client*>()> _client_factory;
  std::optional<pw_client*> _client = std::nullopt;
};

class ProxyWatcher {
public:
  void register_callback(pw_registry *registry);
  std::expected<void, pwcpp::error> watch_props_param(std::string node_name);
  std::optional<Proxy> get_proxy(uint32_t id);

private:
  pw_registry_events registry_events{
    .version = PW_VERSION_REGISTRY_EVENTS, .global = process_registry_event,
  };

  pw_node_events node_events{
    .version = PW_VERSION_NODE_EVENTS, .param = process_node_params_event
  };

  std::mutex watched_params_mutex;
  std::vector<std::string> watched_props_params_nodes;

  std::mutex node_params_mutex;
  std::vector<node_props_params> node_params;

  std::mutex proxies_mutex;
  std::vector<Proxy> proxies;

  pw_registry *registry{nullptr};
  spa_hook registry_hook{};

  static void process_registry_event(void *data, uint32_t id,
                                     uint32_t permissions, const char *c_type,
                                     uint32_t version, const spa_dict *props);

  static void process_node_params_event(void *data, int seq, std::uint32_t id,
                                        uint32_t index, uint32_t next,
                                        const spa_pod *param);

  std::optional<pw_client*> get_proxy_client(uint32_t id);
};
}
