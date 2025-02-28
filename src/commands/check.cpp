#include <iostream>
#include <pmx/constants.h>
#include <wpcpp/link_collection.h>
#include <wpcpp/proxy_collection.h>

#include "console/commands.h"

std::expected<void, error::error> console::check_command(
  std::istringstream &stream, wpcpp::ProxyCollection &proxy_collection,
  wpcpp::LinkCollection &link_collection) {
  std::string sub_command;
  if (stream >> sub_command) {
    if (sub_command == "osc") {
    }
  } else {
    return std::unexpected(
      error::error::invalid_argument("unknown sub command " + sub_command));
  }
  return {};
}
