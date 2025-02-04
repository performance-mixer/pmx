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
  auto config_directory = get_config_directory(application_name);
  create_directory_if_not_exists(config_directory);
  auto config_file_path = config_directory + "/config.yaml";

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
      auto node = config_yaml["input_channels"][
        Prefix::CHANNEL_PORT_PREFIX + std::to_string(i + 1)];
      if (node && !node.IsNull()) {
        config.input_channels[i] = node.as<std::string>();
      } else {
        config.input_channels[i] = std::nullopt;
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
