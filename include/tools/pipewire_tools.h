#pragma once

#include <string>
#include <variant>
#include <optional>
#include <span>

#include <pipewire/main-loop.h>
#include <pipewire/node.h>

struct pw_client;

namespace tools {
using param_value_variant = std::variant<
  double, std::string, float, bool, int, std::nullopt_t>;

void set_props_param(pw_client *client, pw_main_loop *loop,
                     const std::string &parameter_name, double value);

void set_props_param(pw_client *client, pw_main_loop *loop,
                     std::span<std::tuple<std::string, param_value_variant>>
                     parameters);
}
