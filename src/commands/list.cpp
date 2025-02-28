#include <expected>
#include <iostream>
#include <sstream>
#include <error/error.h>
#include <sdcpp/bus.h>
#include <sdcpp/units.h>

#include "console/commands.h"

std::expected<void, error::error> console::list_command(
  std::istringstream &stream, wpcpp::LinkCollection &link_collection,
  wpcpp::ProxyCollection &proxy_collection, sdcpp::Bus &bus) {
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
    } else if (sub_command == "unit_file") {
      auto unit_files = sdcpp::list_unit_files(bus);
      if (unit_files.has_value()) {
        for (const auto &unit_file : unit_files.value()) {
          std::cout << unit_file << std::endl;
        }
        return {};
      } else {
        return std::unexpected(
          error::error::systemd(unit_files.error().message));
      }
    } else {
      return std::unexpected(
        error::error::invalid_argument("unknown sub command " + sub_command));
    }
  }
  return std::unexpected(error::error::invalid_argument("missing sub-command"));
}
