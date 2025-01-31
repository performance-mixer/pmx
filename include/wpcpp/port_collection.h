#pragma once

#include "port.h"

#include <vector>
#include <mutex>

#include <wp/wp.h>

namespace wpcpp {
class PortCollection {
public:
  void push_back(const pipewire_port &port) {
    std::lock_guard lock(_mutex);
    _ports.push_back(port);
  }

  std::vector<pipewire_port> get_ports() {
    std::lock_guard lock(_mutex);
    std::vector<pipewire_port> result_ports;
    std::copy(_ports.begin(), _ports.end(), std::back_inserter(result_ports));
    return result_ports;
  }

  void setup(WpObjectManager * object_manager);

private:
  std::vector<pipewire_port> _ports;
  std::mutex _mutex;
};
}
