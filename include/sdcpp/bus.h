#pragma once

#include <systemd/sd-bus.h>

namespace sdcpp {
class Bus {
public:
  explicit Bus(sd_bus *sd_bus): _sd_bus(sd_bus) {}

  sd_bus *bus() { return _sd_bus; }

private:
  sd_bus *_sd_bus;
};
}
