#include "osc/parse_osc_path.h"
#include "parameters/parameters.h"
#include <cctype>
#include <ranges>
#include <bits/fs_fwd.h>

osc::osc_path osc::parse_osc_path(const std::string &path) {
  if (path.substr(0, 5) == "/I/A/") {
    size_t channel(0);
    size_t consumed(5);
    if (std::isdigit(path[consumed + 1])) {
      channel = std::stoi(path.substr(consumed, 2));
      consumed += 3;
    } else {
      channel = std::stoi(path.substr(consumed, 1));
      consumed += 2;
    };

    auto device_short_name = path.substr(consumed, 3);
    consumed += 4;

    const auto parameter_name = path.substr(consumed);

    auto parameter = std::ranges::find_if(parameters::all,
                                          [&device_short_name, &parameter_name](
                                          const auto &parameter) {
                                            return parameter->device_short_name
                                              == device_short_name && parameter
                                              ->name == parameter_name;
                                          });

    return {channel, *parameter};
  }

  return {0, parameters::none};
}
