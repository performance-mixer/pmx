#include "config/config.h"
#include "config/prefix.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>

const std::string initial_config(R"(
input_channels:
  channel_port_1: null
  channel_port_2: null
  channel_port_3: null
  channel_port_4: null
  channel_port_5: null
  channel_port_6: null
  channel_port_7: null
  channel_port_8: null
  channel_port_9: null
  channel_port_10: null
  channel_port_11: null
  channel_port_12: null
  channel_port_13: null
  channel_port_14: null
  channel_port_15: null
  channel_port_16: null
  channel_port_17: null
  channel_port_18: null
  channel_port_19: null
  channel_port_20: null
  channel_port_21: null
  channel_port_22: null
  channel_port_23: null
  channel_port_24: null
  channel_port_25: null
  channel_port_26: null
  channel_port_27: null
  channel_port_28: null
  channel_port_29: null
  channel_port_30: null
  channel_port_31: null
  channel_port_32: null
input_channel_groups:
  channel_group_1: 1
  channel_group_2: 1
  channel_group_3: 1
  channel_group_4: 1
  channel_group_5: 1
  channel_group_6: 1
  channel_group_7: 1
  channel_group_8: 1
  channel_group_9: 1
  channel_group_10: 1
  channel_group_11: 1
  channel_group_12: 1
  channel_group_13: 1
  channel_group_14: 1
  channel_group_15: 1
  channel_group_16: 1
  channel_group_17: 1
  channel_group_18: 1
  channel_group_19: 1
  channel_group_20: 1
  channel_group_21: 1
  channel_group_22: 1
  channel_group_23: 1
  channel_group_24: 1
  channel_group_25: 1
  channel_group_26: 1
  channel_group_27: 1
  channel_group_28: 1
  channel_group_29: 1
  channel_group_30: 1
  channel_group_31: 1
  channel_group_32: 1
)");

std::string config::get_config_directory(const std::string &application_name) {
  auto config_dir = std::getenv("XDG_CONFIG_HOME");
  if (config_dir) {
    return std::string(config_dir) + "/" + application_name;
  }

  auto home_dir = std::getenv("HOME");
  if (home_dir) {
    return std::string(home_dir) + "/.config/" + application_name;
  }

  throw std::runtime_error("Could not find config directory");
}

bool config::create_directory_if_not_exists(const std::string &path) {
  if (std::filesystem::exists(path)) {
    return true;
  }

  return std::filesystem::create_directory(path);
}

YAML::Node config::read_config_yaml_or_create_if_not_exists(
  const std::string &application_name) {
  const auto config_directory = get_config_directory(application_name);
  create_directory_if_not_exists(config_directory);
  const auto config_file_path = config_directory + "/config.yaml";

  if (std::filesystem::exists(config_file_path)) {
    return YAML::LoadFile(config_file_path);
  }

  std::ofstream config_file(config_file_path);
  if (!config_file.is_open()) {
    throw std::runtime_error(
      "Failed to create config file at " + config_file_path);
  }

  config_file << initial_config;
  config_file.close();

  return YAML::Load(initial_config);
}

config::config config::read_config(const std::string &application_name) {
  auto config_yaml = read_config_yaml_or_create_if_not_exists(application_name);
  config config;

  if (config_yaml["input_channels"]) {
    for (size_t i = 0; i < 32; i++) {
      auto channel_name = Prefix::CHANNEL_PORT_PREFIX + std::to_string(i + 1);
      auto node = config_yaml["input_channels"][channel_name];
      if (node && !node.IsNull()) {
        config.input_channels[i] = node.as<std::string>();
      } else {
        config.input_channels[i] = std::nullopt;
      }
    }
  }

  if (config_yaml["input_channel_groups"]) {
    for (size_t i = 0; i < 32; i++) {
      auto channel_name = Prefix::CHANNEL_GROUP_PREFIX + std::to_string(i + 1);
      auto node = config_yaml["input_channel_groups"][channel_name];
      if (node && !node.IsNull()) {
        config.input_channel_groups[i] = node.as<int>();
      } else {
        config.input_channel_groups[i] = 1;
      }
    }
  }

  return config;
}

bool config::write_config(const std::string &application_name,
                          const config &config) {
  YAML::Node config_yaml;
  config_yaml["input_channels"] = YAML::Node(YAML::NodeType::Map);
  for (size_t i = 0; i < 32; i++) {
    if (config.input_channels[i].has_value()) {
      config_yaml["input_channels"][Prefix::CHANNEL_PORT_PREFIX + std::to_string(i + 1)] = config
        .input_channels[i].value();
    } else {
      config_yaml["input_channels"][Prefix::CHANNEL_PORT_PREFIX + std::to_string(i + 1)] =
        YAML::Node(YAML::NodeType::Null);
    }

    config_yaml["input_channel_groups"][Prefix::CHANNEL_GROUP_PREFIX + std::to_string(i + 1)] =
      static_cast<int>(config.input_channel_groups[i]);
  }

  auto config_directory = get_config_directory(application_name);
  create_directory_if_not_exists(config_directory);
  auto config_file_path = config_directory + "/config.yaml";
  std::ofstream config_file(config_file_path);
  if (!config_file.is_open()) {
    throw std::runtime_error(
      "Failed to create config file at " + config_file_path);
  }

  config_file << config_yaml;
  return true;
}
