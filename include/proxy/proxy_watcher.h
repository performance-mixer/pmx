#pragma once

#include <expected>
#include <boost/asio/detail/mutex.hpp>
#include <utility>
#include <pipewire/pipewire.h>
#include <pwcpp/error.h>

namespace proxy {
enum class proxy_type { node };

class Proxy {
public:
  using parameter_value_variant = std::variant<
    int, float, double, std::string, std::nullopt_t>;

  Proxy(std::function<std::optional<pw_client*>()> client_factory,
        proxy_type type, uint32_t id, std::string name) : type(type), id(id),
                                                          name(std::move(name)),
                                                          _client_factory(
                                                            std::move(
                                                              client_factory)) {
  }

  std::optional<pw_client*> client() {
    if (!_client.has_value()) {
      _client = _client_factory();
    }
    return _client;
  };

  void update_parameters(
    std::span<std::tuple<std::string, parameter_value_variant>> parameters);

  proxy_type type;
  uint32_t id;
  std::string name;
  bool connected = false;
  spa_hook hook{};

  std::vector<std::tuple<std::string, parameter_value_variant>> parameters() {
    return _parameters;
  }

private:
  std::function<std::optional<pw_client*>()> _client_factory;
  std::optional<pw_client*> _client = std::nullopt;

  std::vector<std::tuple<std::string, parameter_value_variant>> _parameters;
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
