#include <algorithm>
#include <expected>
#include <iostream>
#include <sstream>
#include <error/error.h>
#include <wpcpp/link_collection.h>
#include <wpcpp/proxy_collection.h>

#include "console/commands.h"

std::expected<void, error::error> console::describe_command(
  std::istringstream &stream, wpcpp::ProxyCollection &proxy_collection,
  proxy::ProxyWatcher &proxy_watcher, wpcpp::LinkCollection &link_collection) {
  std::string object;
  if (stream >> object) {
    if (std::all_of(object.begin(), object.end(), ::isdigit)) {
      auto id = std::stoi(object);

      auto pw_proxy = proxy_watcher.get_proxy(id);
      if (pw_proxy.has_value()) {
        std::cout << pw_proxy.value()->name << std::endl << std::endl <<
          "Parameters:" << std::endl;
        auto parameters = pw_proxy.value()->parameters();
        for (const auto &parameter : parameters) {
          std::cout << std::get<0>(parameter) << ": ";
          auto value = std::get<1>(parameter);
          if (std::holds_alternative<std::string>(value)) {
            std::cout << std::get<std::string>(value) << std::endl;
          } else if (std::holds_alternative<int>(value)) {
            std::cout << std::get<int>(value) << std::endl;
          } else if (std::holds_alternative<float>(value)) {
            std::cout << std::get<float>(value) << std::endl;
          } else if (std::holds_alternative<double>(value)) {
            std::cout << std::get<double>(value) << std::endl;
          } else {
            std::cout << "unknown type" << std::endl;
          }
        }
      }

      auto proxies = proxy_collection.get_proxies();
      const auto proxy = std::find_if(proxies.begin(), proxies.end(),
                                      [id](const auto &proxy) {
                                        return proxy.object_id == id;
                                      });

      std::cout << std::endl << "Links:" << std::endl;
      auto links = link_collection.get_links();
      if (proxy != proxies.end()) {
        std::cout << proxy->name << std::endl;
        std::vector<wpcpp::link> source_links;
        std::copy_if(links.begin(), links.end(),
                     std::back_inserter(source_links), [id](const auto &link) {
                       return link.output_node_id == id;
                     });

        for (const auto &link : source_links) {
          std::cout << "-->" << link << std::endl;
        }

        std::vector<wpcpp::link> sink_links;
        std::copy_if(links.begin(), links.end(), std::back_inserter(sink_links),
                     [id](const auto &link) {
                       return link.input_node_id == id;
                     });

        for (const auto &link : sink_links) {
          std::cout << "<--" << link << std::endl;
        }
      }

      const auto link = std::find_if(links.begin(), links.end(),
                                     [id](const auto &link) {
                                       return link.object_id == id;
                                     });
      if (link != links.end()) {
        std::cout << *link << std::endl;
      }

      return {};
    } else {
      return std::unexpected(
        error::error::invalid_argument("id is not a valid number: " + object));
    }
  } else {
    return std::unexpected(error::error::invalid_argument("missing object id"));
  }
}
