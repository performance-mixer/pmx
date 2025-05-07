#pragma once

#include <yaml-cpp/yaml.h>

#include <string>

namespace config {
struct config {
  std::array<std::optional<std::string>, 32> input_channels;
  std::array<uint8_t, 32> input_channel_groups{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1
  };
};

std::string get_config_directory(const std::string &application_name);
bool create_directory_if_not_exists(const std::string &path);
YAML::Node read_config_yaml_or_create_if_not_exists(
  const std::string &application_name);
config read_config(const std::string &application_name);
bool write_config(const std::string &application_name, const config &config);
}
