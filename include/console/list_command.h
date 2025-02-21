#pragma once
#include <wpcpp/link_collection.h>

namespace console {
inline std::expected<void, error::error> list_command(
  std::istringstream &stream, wpcpp::LinkCollection &link_collection,
  wpcpp::ProxyCollection &proxy_collection) {
  std::string sub_command;
  if (stream >> sub_command) {
    if (sub_command == "proxy") {
      auto proxies = proxy_collection.get_proxies();
      for (const auto &proxy : proxies) {
        std::cout << proxy.name << ": " << proxy.object_id << std::endl;
      }
      return {};
    } else if (sub_command == "link") {
      auto links = link_collection.get_links();
      for (auto &&link : links) {
        std::cout << link << std::endl;
      }
      return {};
    } else {
      return std::unexpected(
        error::error::invalid_argument("unknown sub command " + sub_command));
    }
  }
  return std::unexpected(error::error::invalid_argument("missing sub-command"));
}
}
