#include <iostream>
#include <tools/filter_parameter_tools.h>
#include <tools/pipewire_tools.h>

#include "console/commands.h"

std::expected<void, error::error> console::setup_command(
  std::istringstream &stream, wpcpp::ProxyCollection &proxy_collection,
  proxy::ProxyWatcher &proxy_watcher, pw_main_loop *loop) {
  auto proxies = proxy_collection.get_proxies();
  auto filter_chain_ctrl_proxy = std::find_if(proxies.begin(), proxies.end(),
                                              [](const auto &proxy) {
                                                return proxy.name ==
                                                  "pmx-filter-chain-ctrl";
                                              });

  if (filter_chain_ctrl_proxy == proxies.end()) {
    return std::unexpected(
      error::error::invalid_argument("couldn't find filter chain ctrl"));
  }

  auto filter_chain_ctrl_id = filter_chain_ctrl_proxy->object_id;
  auto pw_filter_chain_proxy = proxy_watcher.get_proxy(filter_chain_ctrl_id);

  if (!pw_filter_chain_proxy.has_value()) {
    return std::unexpected(
      error::error::invalid_argument(
        "couldn't find filter chain ctrl pw proxy"));
  }

  std::vector<std::tuple<std::string, tools::param_value_variant>> parameters;

  auto layer_proxy = std::find_if(proxies.begin(), proxies.end(),
                                  [](const auto &proxy) {
                                    return proxy.name ==
                                      "pmx-layer-channels-ins";
                                  });

  if (layer_proxy == proxies.end()) {
    std::cout << "no layer proxy found, clearing" << std::endl;
    parameters.emplace_back("layerChannels",
                            tools::param_value_variant{std::nullopt});
  } else {
    auto layer_id = layer_proxy->object_id;
    std::cout << "layer proxy found, " << layer_id << std::endl;
    parameters.emplace_back("layerChannels",
                            tools::param_value_variant{layer_id});
  }

  auto group_a_proxy = std::find_if(proxies.begin(), proxies.end(),
                                    [](const auto &proxy) {
                                      return proxy.name ==
                                        "pmx-group-channels-a-ins";
                                    });

  if (group_a_proxy == proxies.end()) {
    std::cout << "no group a proxy found, clearing" << std::endl;
    parameters.emplace_back("groupChannelsA",
                            tools::param_value_variant{std::nullopt});
  } else {
    auto group_a_id = group_a_proxy->object_id;
    std::cout << "group a proxy found, " << group_a_id << std::endl;
    parameters.emplace_back("groupChannelsA",
                            tools::param_value_variant{group_a_id});
  }

  auto group_b_proxy = std::find_if(proxies.begin(), proxies.end(),
                                    [](const auto &proxy) {
                                      return proxy.name ==
                                        "pmx-group-channels-b-ins";
                                    });

  if (group_b_proxy == proxies.end()) {
    std::cout << "no group b proxy found, clearing" << std::endl;
    parameters.emplace_back("groupChannelsB",
                            tools::param_value_variant{std::nullopt});
  } else {
    auto group_b_id = group_b_proxy->object_id;
    std::cout << "group b proxy found, " << group_b_id << std::endl;
    parameters.emplace_back("groupChannelsB",
                            tools::param_value_variant{group_b_id});
  }

  auto inputs_proxy = std::find_if(proxies.begin(), proxies.end(),
                                   [](const auto &proxy) {
                                     return proxy.name ==
                                       "pmx-input-channels-ins";
                                   });

  if (inputs_proxy == proxies.end()) {
    std::cout << "no inputs proxy found, clearing" << std::endl;
    parameters.emplace_back("inputChannels",
                            tools::param_value_variant{std::nullopt});
  } else {
    auto inputs_id = inputs_proxy->object_id;
    std::cout << "inputs proxy found, " << inputs_id << std::endl;
    parameters.emplace_back("inputChannels",
                            tools::param_value_variant{inputs_id});
  }

  auto client = pw_filter_chain_proxy.value()->client();
  if (client.has_value()) {
    tools::set_props_param(client.value(), loop, parameters);
    return {};
  } else {
    return std::unexpected(error::error::pipewire("couldn't get client"));
  }
}
