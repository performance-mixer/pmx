#include "console/commands.h"

#include <iostream>
#include <metadata/metadata_watcher.h>
#include <wpcpp/metadata.h>

std::expected<void, error::error> console::meta_command(
  std::istringstream &stream, wpcpp::Metadata &metadata) {
  std::string sub_command;
  if (stream >> sub_command) {
    if (sub_command == "show") {
      auto meta_entries = metadata.get_metadata_values();
      for (const auto &entry : meta_entries) {
        std::cout << std::get<0>(entry) << ": " << std::get<1>(entry) <<
          std::endl;
      }
      return {};
    } else {
      return std::unexpected(
        error::error::invalid_argument("unknown sub command " + sub_command));
    }
  } else {
    return std::unexpected(
      error::error::invalid_argument("missing sub command"));
  }
}
