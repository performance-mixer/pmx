#include "pwcpp/filter_app.h"

#include <cstring>
#include <string>

#include <pipewire/loop.h>
#include <pipewire/pipewire.h>

#include <spa/control/control.h>
#include <spa/debug/pod.h>
#include <spa/pod/builder.h>
#include <spa/pod/iter.h>
#include <spa/pod/parser.h>
#include <spa/pod/pod.h>
#include <spa/utils/dict.h>

void pwcpp::FilterApp::on_process(void *user_data,
                                  struct spa_io_position *position) {
  auto this_pointer = static_cast<FilterApp *>(user_data);
}

void pwcpp::FilterApp::handle_filter_param_update(void *user_data,
                                                  void *port_data, uint32_t id,
                                                  const struct spa_pod *pod) {
  auto this_pointer = static_cast<FilterApp *>(user_data);

  struct spa_pod_prop *prop;
  struct spa_pod_object *obj = (struct spa_pod_object *)pod;
  SPA_POD_OBJECT_FOREACH(obj, prop) {
    struct spa_pod_parser parser;
    spa_pod_parser_pod(&parser, &prop->value);

    char *c_key = nullptr;
    char *c_value = nullptr;
    spa_pod_parser_get_struct(&parser, SPA_POD_String(&c_key),
                              SPA_POD_String(&c_value));

    if (c_key == nullptr && c_value == nullptr) {
      return;
    }

    std::string key(c_key);
    std::string value(c_value);
  }
}

static const pw_filter_events filter_events = {
    .version = PW_VERSION_FILTER_EVENTS,
    .param_changed = pwcpp::FilterApp::handle_filter_param_update,
    .process = pwcpp::FilterApp::on_process,
};

static const struct pw_registry_events registry_events = {
    .version = PW_VERSION_REGISTRY_EVENTS,
    .global = pwcpp::FilterApp::registry_event_global,
    .global_remove = pwcpp::FilterApp::registry_event_global_remove,
};

pwcpp::FilterApp::FilterApp() {

  /*
  input_channels_port =
      static_cast<struct processing::port *>(pw_filter_add_port(
          filter, PW_DIRECTION_INPUT, PW_FILTER_PORT_FLAG_MAP_BUFFERS,
          sizeof(struct processing::port),
          pw_properties_new(PW_KEY_FORMAT_DSP, "8 bit raw midi",
                            PW_KEY_PORT_NAME, "input_channels_port", NULL),
          NULL, 0));

  uint8_t buffer[1024];
  struct spa_pod_builder builder;
  struct spa_pod *params[1];
  spa_pod_builder_init(&builder, buffer, sizeof(buffer));

  params[0] = static_cast<struct spa_pod *>(spa_pod_builder_add_object(
      &builder,
      SPA_TYPE_OBJECT_ParamBuffers, SPA_PARAM_Buffers,
      SPA_PARAM_BUFFERS_buffers, SPA_POD_CHOICE_RANGE_Int(1, 1, 32),
      SPA_PARAM_BUFFERS_blocks, SPA_POD_Int(1),
      SPA_PARAM_BUFFERS_size, SPA_POD_CHOICE_RANGE_Int(4096, 4096, INT32_MAX),
      SPA_PARAM_BUFFERS_stride, SPA_POD_Int(1)));

  pw_filter_update_params(filter, updates_port, (const struct spa_pod **)params,
                          SPA_N_ELEMENTS(params));
  */
}

int pwcpp::FilterApp::run() {
  if (pw_filter_connect(filter, PW_FILTER_FLAG_RT_PROCESS, NULL, 0) < 0) {
    fprintf(stderr, "can't connect\n");
    return -1;
  }

  pw_main_loop_run(loop);
  pw_proxy_destroy((struct pw_proxy *)registry);
  pw_core_disconnect(core);
  pw_filter_destroy(filter);
  pw_filter_destroy(filter);
  pw_main_loop_destroy(loop);
  pw_deinit();

  return 0;
}

void pwcpp::FilterApp::initialize(int argc, char *argv[]) {
  pw_init(&argc, &argv);

  loop = pw_main_loop_new(NULL);

  pw_loop_add_signal(pw_main_loop_get_loop(loop), SIGINT, do_quit, this);
  pw_loop_add_signal(pw_main_loop_get_loop(loop), SIGTERM, do_quit, this);

  context = pw_context_new(pw_main_loop_get_loop(loop), nullptr, 0);
  core = pw_context_connect(context, nullptr, 0);
  registry = pw_core_get_registry(core, PW_VERSION_REGISTRY, 0);

  spa_zero(registry_listener);
  pw_registry_add_listener(registry, &registry_listener, &registry_events,
                           this);

  filter = pw_filter_new_simple(
      pw_main_loop_get_loop(loop), "fr-pmx-cmd-router",
      pw_properties_new(PW_KEY_MEDIA_TYPE, "Midi", PW_KEY_MEDIA_CATEGORY,
                        "Filter", PW_KEY_MEDIA_CLASS, "Midi/Sink",
                        PW_KEY_MEDIA_ROLE, "DSP", "pmx.channel_mapping", "",
                        NULL),
      &filter_events, this);
}

void pwcpp::FilterApp::shutdown() { pw_main_loop_quit(loop); }

void pwcpp::FilterApp::do_quit(void *user_data, int signal_number) {
  auto this_pointer = static_cast<FilterApp *>(user_data);
  this_pointer->shutdown();
}

void pwcpp::FilterApp::registry_event_global(void *data, uint32_t id,
                                             uint32_t permissions,
                                             const char *c_type,
                                             uint32_t version,
                                             const struct spa_dict *props) {
  auto this_pointer = static_cast<FilterApp *>(data);

  std::string type(c_type);
  if (type == "PipeWire:Interface:Node") {
    auto client = static_cast<pw_client *>(pw_registry_bind(
        this_pointer->registry, id, c_type, PW_VERSION_CLIENT, 0));
    this_pointer->node_registry.add_node(id, client);
  }
}

void pwcpp::FilterApp::registry_event_global_remove(void *data, uint32_t id) {
  auto this_pointer = static_cast<FilterApp *>(data);

  this_pointer->node_registry.delete_node_by_id(id);
}
