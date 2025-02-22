#pragma once

#include <wpcpp/link_collection.h>
#include <wpcpp/proxy_collection.h>
#include <systemd/sd-bus.h>

namespace console {
inline std::expected<void, error::error> pmx_command(
  std::istringstream &stream, wpcpp::Metadata &metadata,
  wpcpp::ProxyCollection &proxy_collection,
  wpcpp::LinkCollection &link_collection, sd_bus *sd_bus) {
  std::string sub_command;
  if (stream >> sub_command) {
    if (sub_command == "enable") {
      metadata.set_metadata_value("enabled", "true");
      return {};
    } else if (sub_command == "disable") {
      metadata.set_metadata_value("enabled", "false");
    } else if (sub_command == "list") {
      std::string object;
      if (stream >> object) {
        if (object == "node") {
          for (auto &&proxy : proxy_collection.get_proxies()) {
            if (proxy.name.starts_with("pmx-") && proxy.type ==
              wpcpp::proxy_type::node) {
              std::cout << proxy.name << ": " << proxy.object_id << std::endl;
            }
          }
          return {};
        } else {
          return std::unexpected(
            error::error::invalid_argument(
              "unknown or unimplemented object" + object));
        }
      } else {
        return std::unexpected(
          error::error::invalid_argument("missing object"));
      }
    } else {
      return std::unexpected(
        error::error::invalid_argument("unknown sub command " + sub_command));
    }
  }

  return std::unexpected(error::error::invalid_argument("missing sub command"));
}
}
