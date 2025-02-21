#pragma once

namespace console {
inline std::expected<void, error::error> describe_command(
  std::istringstream &stream, wpcpp::ProxyCollection &proxy_collection,
  wpcpp::LinkCollection &link_collection) {
  std::string object;
  if (stream >> object) {
    if (std::all_of(object.begin(), object.end(), ::isdigit)) {
      auto id = std::stoi(object);

      auto proxies = proxy_collection.get_proxies();
      auto proxy = std::find_if(proxies.begin(), proxies.end(),
                                [id](const auto &proxy) {
                                  return proxy.object_id == id;
                                });

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

      auto link = std::find_if(links.begin(), links.end(),
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
}
