#include "osc/parse_osc_path.h"
#include "parameters/parameters.h"
#include "error/error.h"

#include <cctype>
#include <expected>
#include <ranges>

std::expected<osc::osc_path, error::error> osc::parse_osc_path(
  const std::string &path) {
  if (path.size() < 7) {
    return std::unexpected(
      error::error::invalid_argument("osc path too short"));
  }

  osc_path_channel_type channel_type;
  switch (path[1]) {
  case 'I':
    channel_type = osc_path_channel_type::input;
    break;
  case 'G':
    channel_type = osc_path_channel_type::group;
    break;
  case 'L':
    channel_type = osc_path_channel_type::layer;
    break;
  default:
    return std::unexpected(
      error::error::invalid_argument("invalid channel type " + path[1]));
  }

  if (path[3] != 'A' && path[3] != 'B') {
    return std::unexpected(
      error::error::invalid_argument("invalid layer " + path[3]));
  }

  const auto layer = path[3] == 'A' ? osc_path_layer::A : osc_path_layer::B;

  size_t consumed(5);
  size_t channel(0);
  if (std::isdigit(path[5]) && std::isdigit(path[6])) {
    channel = std::stoi(path.substr(5, 2));
    consumed += 3;
  } else if (std::isdigit(path[5])) {
    channel = std::stoi(path.substr(5, 1));
    consumed += 2;
  } else {
    return std::unexpected(error::error::invalid_argument("invalid channel"));
  }

  auto device_short_name = path.substr(consumed, 3);
  consumed += 4;

  const auto parameter_name = path.substr(consumed);

  const auto parameter = std::ranges::find_if(parameters::all,
                                        [&device_short_name, &parameter_name](
                                        const auto &parameter) {
                                          return parameter->device_short_name ==
                                            device_short_name && parameter->name
                                            == parameter_name;
                                        });

  if (parameter == parameters::all.end()) {
    return std::unexpected(error::error::invalid_argument("unknown parameter"));
  }

  return osc::osc_path{channel, channel_type, layer, *parameter};
}
