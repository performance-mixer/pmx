#pragma once

#include "pwcpp/node_registry.h"

#include <cstddef>
#include <functional>
#include <pipewire/context.h>
#include <pipewire/filter.h>
#include <pipewire/main-loop.h>

namespace pwcpp {

class FilterApp {
public:
  template <class MessageType>
  using ByteReader =
      std::function<MessageType(FilterApp *this_pointer, char *, size_t)>;

  using ByteWriter = std::function<void(const char *, size_t)>;

  using PortPodWriter = std::function<void(ByteWriter &)>;

  enum PortDirection { INPUT, OUTPUT };

  enum PortType { MIDI, OSC };

  FilterApp();

  static void registry_event_global(void *data, uint32_t id,
                                    uint32_t permissions, const char *c_type,
                                    uint32_t version,
                                    const struct spa_dict *props);

  static void registry_event_global_remove(void *data, uint32_t id);

  static void on_process(void *user_data, struct spa_io_position *position);

  static void handle_filter_param_update(void *data, void *port_data,
                                         uint32_t id,
                                         const struct spa_pod *pod);

  static void do_quit(void *user_data, int signal_number);

  void initialize(int argc, char *argv[]);

  void shutdown();

  int run();

private:
  struct pw_main_loop *loop;
  struct pw_context *context;
  struct pw_core *core;
  struct pw_registry *registry;
  struct pw_filter *filter;
  struct spa_hook registry_listener;
  NodeRegistry node_registry;
};

} // namespace pwcpp
