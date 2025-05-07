#include <iostream>
#include <console/colors.h>
#include <pmx/constants.h>
#include <sdcpp/units.h>

#include "console/commands.h"

std::expected<void, sdcpp::error> check_systemd_unit_status(sdcpp::Bus &bus) {
  auto units = sdcpp::list_units(bus);
  auto unit_files = sdcpp::list_unit_files(bus);
  if (units.has_value() && unit_files.has_value()) {
    for (auto &unit_name : pmx::constants::all_services) {
      auto unit = std::find_if(units.value().begin(), units.value().end(),
                               [unit_name](const auto &unit) {
                                 return unit.name == unit_name;
                               });
      std::string reset_color = console::colors::reset;
      if (unit != units->end()) {
        std::cout << unit->name << " ";

        std::string color;
        if (unit->active_state == "active") {
          color = console::colors::green;
        } else if (unit->active_state == "failed") {
          color = console::colors::red;
        }

        std::cout << color << unit->active_state << reset_color << std::endl;
      } else {
        auto unit_file = std::find_if(unit_files.value().begin(),
                                      unit_files.value().end(),
                                      [unit_name](const auto &unit_file) {
                                        return unit_file.name.ends_with(
                                          unit_name);
                                      });
        if (unit_file != unit_files->end()) {
          std::string color;
          if (unit_file->status == "disabled") {
            color = console::colors::red;
          } else {
            color = console::colors::yellow;
          }
          std::cout << unit_name << " " << color << " " << unit_file->status <<
            reset_color << std::endl;
        } else {
          std::cout << unit_name << " " << console::colors::red << "not found"
            << reset_color << std::endl;
        }
      }
    }

    return {};
  } else {
    if (!units.has_value()) {
      return std::unexpected(units.error());
    } else {
      return std::unexpected(unit_files.error());
    }
  }
}

std::expected<void, error::error> check_osc_connections(
  wpcpp::ProxyCollection &proxy_collection,
  wpcpp::LinkCollection &link_collection) {
  auto proxies = proxy_collection.get_proxies();
  std::vector<wpcpp::proxy> producer_nodes;
  std::vector<wpcpp::proxy> consumer_nodes;
  for (const auto &proxy : proxies) {
    if (std::find(pmx::constants::pmx_osc_producer_node_names.begin(),
                  pmx::constants::pmx_osc_producer_node_names.end(),
                  proxy.name) != pmx::constants::pmx_osc_producer_node_names.
      end()) {
      if (proxy.type == wpcpp::proxy_type::node) {
        producer_nodes.push_back(proxy);
        continue;
      }
    }

    if (std::find(pmx::constants::pmx_osc_consumer_node_names.begin(),
                  pmx::constants::pmx_osc_consumer_node_names.end(),
                  proxy.name) != pmx::constants::pmx_osc_consumer_node_names.
      end()) {
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
                                         [output_node_id](const auto &proxy) {
                                           return proxy.type ==
                                             wpcpp::proxy_type::port && proxy.
                                             name == "pmx-osc" && proxy.node_id.
                                                                        value()
                                             == output_node_id;
                                         });

    if (output_port_iter == proxies.end()) {
      std::cout << "couldn't find output port for node " << std::get<
        0>(expected_link).name << std::endl;
      continue;
    }

    auto input_port_iter = std::find_if(proxies.begin(), proxies.end(),
                                        [input_node_id ](const auto &proxy) {
                                          return proxy.type ==
                                            wpcpp::proxy_type::port && proxy.
                                            name == "pmx-osc" && proxy.node_id.
                                                                       value()
                                            == input_node_id;
                                        });

    if (input_port_iter == proxies.end()) {
      std::cout << "couldn't find input port for node " << std::get<
        1>(expected_link).name << std::endl;
      continue;
    }

    auto link = std::find_if(links.begin(), links.end(),
                             [output_port_iter,input_port_iter ](
                             const auto &link) {
                               return link.output_port_id == output_port_iter->
                                 object_id && link.input_port_id ==
                                 input_port_iter->object_id;
                             });

    if (link == links.end()) {
      std::cout << "couldn't find link between " << std::get<0>(expected_link).
        name << " and " << std::get<1>(expected_link).name << std::endl;
    }
  }

  return {};
}

std::expected<void, sdcpp::error> console::status_command(
  wpcpp::ProxyCollection &proxy_collection,
  wpcpp::LinkCollection &link_collection, sdcpp::Bus &bus) {
  auto systemd_check_result = check_systemd_unit_status(bus);
  auto osc_check_result = check_osc_connections(proxy_collection,
                                                link_collection);
  return {};
}
