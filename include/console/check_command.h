#pragma once

#include "pmx/constants.h"

namespace console {
inline std::expected<void, error::error> check_command(
  std::istringstream &stream, wpcpp::ProxyCollection &proxy_collection,
  wpcpp::LinkCollection &link_collection) {
  std::string sub_command;
  if (stream >> sub_command) {
    if (sub_command == "osc") {
      auto proxies = proxy_collection.get_proxies();
      std::vector<wpcpp::proxy> producer_nodes;
      std::vector<wpcpp::proxy> consumer_nodes;
      for (const auto &proxy : proxies) {
        if (std::find(pmx::constants::pmx_osc_producer_node_names.begin(),
                      pmx::constants::pmx_osc_producer_node_names.end(),
                      proxy.name) != pmx::constants::pmx_osc_producer_node_names
          .end()) {
          if (proxy.type == wpcpp::proxy_type::node) {
            producer_nodes.push_back(proxy);
            continue;
          }
        }

        if (std::find(pmx::constants::pmx_osc_consumer_node_names.begin(),
                      pmx::constants::pmx_osc_consumer_node_names.end(),
                      proxy.name) != pmx::constants::pmx_osc_consumer_node_names
          .end()) {
          if (proxy.type == wpcpp::proxy_type::node) {
            consumer_nodes.push_back(proxy);
            continue;
          }
        }
      }

      for (const auto &producer_name :
           pmx::constants::pmx_osc_producer_node_names) {
        auto producer_iter = std::find_if(producer_nodes.begin(),
                                          producer_nodes.end(),
                                          [producer_name ](const auto &proxy) {
                                            return proxy.name == producer_name;
                                          });
        if (producer_iter == producer_nodes.end()) {
          std::cout << "couldn't find producer " << producer_name << std::endl;
        }
      }

      for (const auto &consumer_name :
           pmx::constants::pmx_osc_consumer_node_names) {
        auto consumer_iter = std::find_if(consumer_nodes.begin(),
                                          consumer_nodes.end(),
                                          [consumer_name ](const auto &proxy) {
                                            return proxy.name == consumer_name;
                                          });
        if (consumer_iter == consumer_nodes.end()) {
          std::cout << "couldn't find consumer " << consumer_name << std::endl;
        }
      }

      std::vector<std::tuple<wpcpp::proxy, wpcpp::proxy>> expected_links;
      for (const auto &producer : producer_nodes) {
        for (const auto &consumer : consumer_nodes) {
          expected_links.push_back(std::make_tuple(producer, consumer));
        }
      }

      auto links = link_collection.get_links();
      for (const auto &expected_link : expected_links) {
        auto output_node_id = std::get<0>(expected_link).object_id;
        auto input_node_id = std::get<1>(expected_link).object_id;

        auto output_port_iter = std::find_if(proxies.begin(), proxies.end(),
                                             [output_node_id](
                                             const auto &proxy) {
                                               return proxy.type ==
                                                 wpcpp::proxy_type::port &&
                                                 proxy.name == "pmx-osc" &&
                                                 proxy.node_id.value() ==
                                                 output_node_id;
                                             });

        if (output_port_iter == proxies.end()) {
          std::cout << "couldn't find output port for node " << std::get<
            0>(expected_link).name << std::endl;
          continue;
        }

        auto input_port_iter = std::find_if(proxies.begin(), proxies.end(),
                                            [input_node_id ](
                                            const auto &proxy) {
                                              return proxy.type ==
                                                wpcpp::proxy_type::port && proxy
                                                .name == "pmx-osc" && proxy.
                                                                      node_id.
                                                                      value() ==
                                                input_node_id;
                                            });

        if (input_port_iter == proxies.end()) {
          std::cout << "couldn't find input port for node " << std::get<
            1>(expected_link).name << std::endl;
          continue;
        }

        auto link = std::find_if(links.begin(), links.end(),
                                 [output_port_iter,input_port_iter ](
                                 const auto &link) {
                                   return link.output_port_id ==
                                     output_port_iter->object_id && link.
                                     input_port_id == input_port_iter->
                                     object_id;
                                 });

        if (link == links.end()) {
          std::cout << "couldn't find link between " << std::get<
              0>(expected_link).name << " and " << std::get<1>(expected_link).
            name
            << std::endl;
        }
      }
    }
  } else {
    return std::unexpected(
      error::error::invalid_argument("unknown sub command " + sub_command));
  }
  return {};
}
}
