#pragma once

#include <string>

namespace wpcpp {
struct pipewire_port {
  int id;
  std::string name;
  std::string alias;
  std::string group;
  std::string path;
  std::string dsp_format;
  int node_id;

  enum class Direction { IN, OUT } direction;

  bool physical;
  int object_serial;
};
} // namespace wpcpp
