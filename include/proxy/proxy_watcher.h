#pragma once

#include <expected>
#include <memory>
#include <boost/asio/detail/mutex.hpp>
#include <utility>
#include <error/error.h>
#include <pipewire/pipewire.h>

namespace proxy {
enum class proxy_type { node };

class Proxy {
public:
  using parameter_value_variant = std::variant<
    int, float, double, std::string, std::nullopt_t>;

  proxy_type type;
  uint32_t id;
  std::string name;
  bool connected = false;
  spa_hook hook{};

  Proxy(std::function<std::optional<pw_client*>()> client_factory,
        const proxy_type type, const uint32_t id,
        std::string name) : type(type), id(id), name(std::move(name)),
                            _client_factory(std::move(client_factory)) {}

  std::optional<pw_client*> client() {
    if (!_client.has_value()) {
      _client = _client_factory();
    }
    return _client;
  };

  void update_parameters(
    std::span<std::tuple<std::string, parameter_value_variant>> parameters);

  std::vector<std::tuple<std::string, parameter_value_variant>> parameters() {
    return _parameters;
  }

  std::expected<void, error::error> watch_proxy_prop_params(
    const std::function<void(
      std::span<std::tuple<std::string, parameter_value_variant>>)> &callback) {
    _watch_callbacks.push_back(callback);
    return {};
  }

private:
  std::function<std::optional<pw_client*>()> _client_factory;
  std::optional<pw_client*> _client = std::nullopt;

  std::vector<std::tuple<std::string, parameter_value_variant>> _parameters;

  std::vector<std::function<void(
    std::span<std::tuple<std::string, parameter_value_variant>>)>>
  _watch_callbacks;
};

class ProxyWatcher {
public:
  void register_callback(pw_registry *registry);
  std::optional<Proxy> get_proxy(std::uint32_t id);

private:
  pw_registry_events registry_events{
    .version = PW_VERSION_REGISTRY_EVENTS, .global = process_registry_event,
  };

  pw_node_events node_events{
    .version = PW_VERSION_NODE_EVENTS, .param = process_node_params_event
  };

  std::mutex proxies_mutex;
  std::vector<std::shared_ptr<Proxy>> proxies;

  pw_registry *registry{nullptr};
  spa_hook registry_hook{};

  static void process_registry_event(void *data, uint32_t id,
                                     uint32_t permissions, const char *c_type,
                                     uint32_t version, const spa_dict *props);

  static void process_node_params_event(void *data, int seq, std::uint32_t id,
                                        uint32_t index, uint32_t next,
                                        const spa_pod *param);

  [[nodiscard]] std::optional<pw_client*> get_proxy_client(uint32_t id) const;
};
}
