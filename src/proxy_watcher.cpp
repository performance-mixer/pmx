#include "proxy/proxy_watcher.h"

#include <iostream>
#include <memory>
#include <shared_mutex>
#include <spa/debug/pod.h>
#include <spa/pod/parser.h>

void proxy::ProxyWatcher::register_callback(pw_registry *registry) {
  spa_zero(registry_hook);
  pw_registry_add_listener(registry, &registry_hook, &registry_events, this);
  this->registry = registry;
}

std::optional<pw_client*> proxy::ProxyWatcher::get_proxy_client(
  const uint32_t id) const {
  auto client = static_cast<pw_client*>(pw_registry_bind(
    registry, id, "PipeWire:Interface:Node", PW_VERSION_CLIENT, 0));
  if (client == nullptr) { return std::nullopt; }
  return client;
}

void proxy::ProxyWatcher::process_registry_event(void *data, uint32_t id,
                                                 uint32_t permissions,
                                                 const char *c_type,
                                                 uint32_t version,
                                                 const spa_dict *props) {
  auto *self = static_cast<ProxyWatcher*>(data);
  std::string_view type(c_type);
  if (type == "PipeWire:Interface:Node") {
    const auto name = spa_dict_lookup(props, PW_KEY_NODE_NAME);
    if (name == nullptr) { return; }

    std::lock_guard lock(self->proxies_mutex);
    auto factory = [self, id]() { return self->get_proxy_client(id); };
    auto proxy = std::make_shared<Proxy>(factory, proxy_type::node, id,
                                         std::string(name));
    self->proxies.push_back(proxy);
    auto client = proxy->client();
    if (client.has_value()) {
      pw_node_add_listener(client.value(), &proxy->hook, &self->node_events,
                           proxy.get());
      std::uint32_t params[] = {SPA_PARAM_Props};
      pw_node_subscribe_params(client.value(), params, 1);
    }
  }
};

void proxy::ProxyWatcher::process_node_params_event(void *data, int seq,
                                                    std::uint32_t id,
                                                    std::uint32_t index,
                                                    std::uint32_t next,
                                                    const spa_pod *param) {
  const spa_pod_prop *property = spa_pod_find_prop(
    param, nullptr, SPA_PROP_params);

  if (property == nullptr) { return; }

  void *struct_field_void;
  std::vector<pwcpp::property::property_value_type> values;
  std::vector<std::string> keys;
  std::size_t count(0);
  SPA_POD_STRUCT_FOREACH(&property->value, struct_field_void) {
    auto struct_field = reinterpret_cast<spa_pod*>(struct_field_void);
    if (count % 2 == 0) {
      const char *key;
      spa_pod_get_string(struct_field, &key);
      keys.emplace_back(key);
    } else {
      auto type = SPA_POD_TYPE(struct_field);
      switch (type) {
      case SPA_TYPE_Int:
        int int_value;
        spa_pod_get_int(struct_field, &int_value);
        values.emplace_back(int_value);
        break;
      case SPA_TYPE_Float:
        float float_value;
        spa_pod_get_float(struct_field, &float_value);
        values.emplace_back(float_value);
        break;
      case SPA_TYPE_Double:
        double double_value;
        spa_pod_get_double(struct_field, &double_value);
        values.emplace_back(double_value);
        break;
      case SPA_TYPE_String:
        {
          const char *string_value;
          spa_pod_get_string(struct_field, &string_value);
          if (string_value == nullptr) {
            values.emplace_back(std::nullopt);
          } else {
            values.emplace_back(string_value);
          }
          break;
        }
      default:
        keys.pop_back();
      }
    }

    count++;
  }

  auto *proxy = static_cast<Proxy*>(data);

  std::vector<std::tuple<std::string, pwcpp::property::property_value_type>>
    result;
  for (auto i = 0; i < keys.size(); i++) {
    result.emplace_back(keys[i], values[i]);
  }

  proxy->update_parameters(result);
}

std::optional<std::shared_ptr<proxy::Proxy>>
proxy::ProxyWatcher::get_proxy(uint32_t id) {
  auto proxy = std::find_if(proxies.begin(), proxies.end(),
                            [id](const auto &proxy) {
                              return proxy->id == id;
                            });
  if (proxy != proxies.end()) {
    return *proxy;
  }
  return std::nullopt;
}

bool equal(std::tuple<std::string, pwcpp::property::property_value_type> left,
           std::tuple<std::string, pwcpp::property::property_value_type>
           right) {
  if (std::get<0>(left) != std::get<0>(right)) { return false; }

  if (std::holds_alternative<int>(std::get<1>(left)) && std::holds_alternative<
    int>(std::get<1>(right))) {
    return std::get<int>(std::get<1>(left)) == std::get<
      int>(std::get<1>(right));
  } else if (std::holds_alternative<float>(std::get<1>(left)) &&
    std::holds_alternative<float>(std::get<1>(right))) {
    return std::get<float>(std::get<1>(left)) == std::get<float>(
      std::get<1>(right));
  } else if (std::holds_alternative<double>(std::get<1>(left)) &&
    std::holds_alternative<double>(std::get<1>(right))) {
    return std::get<double>(std::get<1>(left)) == std::get<double>(
      std::get<1>(right));
  } else if (std::holds_alternative<std::string>(std::get<1>(left)) &&
    std::holds_alternative<std::string>(std::get<1>(right))) {
    return std::get<std::string>(std::get<1>(left)) == std::get<std::string>(
      std::get<1>(right));
  } else if (std::holds_alternative<bool>(std::get<1>(left)) &&
    std::holds_alternative<bool>(std::get<1>(right))) {
    return std::get<bool>(std::get<1>(left)) == std::get<bool>(
      std::get<1>(right));
  } else if (std::holds_alternative<std::nullopt_t>(std::get<1>(left)) &&
    std::holds_alternative<std::nullopt_t>(std::get<1>(right))) {
    return true;
  }

  return false;
}

void proxy::Proxy::update_parameters(
  std::span<std::tuple<std::string, pwcpp::property::property_value_type>>
  parameters) {
  std::vector<std::tuple<std::string, pwcpp::property::property_value_type>>
    changes;
  for (auto &parameter : parameters) {
    auto existing_parameter = std::find_if(_parameters.begin(),
                                           _parameters.end(),
                                           [parameter](const auto &p) {
                                             return std::get<0>(p) == std::get<
                                               0>(parameter);
                                           });

    if (existing_parameter != _parameters.end()) {
      if (!equal(*existing_parameter, parameter)) {
        changes.push_back(parameter);
        *existing_parameter = parameter;
      }
    } else {
      changes.push_back(parameter);
      _parameters.push_back(parameter);
    }
  }

  for (auto &&watcher : _watch_callbacks) {
    watcher(changes);
  }
}
