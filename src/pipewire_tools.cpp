#include "tools/pipewire_tools.h"

#include <stdexcept>
#include <vector>
#include <spa/param/param.h>
#include <spa/param/props.h>
#include <spa/pod/builder.h>
#include <pipewire/loop.h>

struct pw_invoke_set_param_data {
  pw_client *client;
  uint8_t buffer[1024];
  spa_pod *pod;
};

spa_pod *build_set_params_message(u_int8_t *buffer, const size_t buffer_size,
                                  const std::string &param_name,
                                  const double param_value) {
  spa_pod_builder builder{};
  spa_pod_builder_init(&builder, buffer, buffer_size);

  spa_pod_frame object_frame{};
  spa_pod_builder_push_object(&builder, &object_frame, SPA_TYPE_OBJECT_Props,
                              SPA_PARAM_Props);
  spa_pod_builder_prop(&builder, SPA_PROP_params, 0);

  spa_pod_frame struct_frame{};
  spa_pod_builder_push_struct(&builder, &struct_frame);
  spa_pod_builder_string(&builder, param_name.c_str());
  spa_pod_builder_double(&builder, param_value);
  spa_pod_builder_pop(&builder, &struct_frame);

  return static_cast<spa_pod*>(spa_pod_builder_pop(&builder, &object_frame));
}

void tools::set_props_param(pw_client *client, pw_main_loop *loop,
                            const std::string &parameter_name,
                            const double value) {
  pw_invoke_set_param_data data{.client = client,};
  data.pod = build_set_params_message(data.buffer, 1024, parameter_name, value);

  auto func = [](struct spa_loop *loop, bool async, u_int32_t seq,
                 const void *data, size_t size, void *user_data) -> int {
    const auto invoke_data = static_cast<const pw_invoke_set_param_data*>(data);
    pw_node_set_param(reinterpret_cast<pw_node *>(invoke_data->client),
                      SPA_PARAM_Props, 0, invoke_data->pod);
    return 0;
  };

  pw_loop_invoke(pw_main_loop_get_loop(loop), func, 0, &data,
                 sizeof(pw_invoke_set_param_data), true, nullptr);
}

spa_pod *build_set_params_message(u_int8_t *buffer, const size_t buffer_size,
                                  std::span<std::tuple<
                                    std::string, tools::param_value_variant>>
                                  parameters) {
  spa_pod_builder builder{};
  spa_pod_builder_init(&builder, buffer, buffer_size);

  spa_pod_frame object_frame{};
  spa_pod_builder_push_object(&builder, &object_frame, SPA_TYPE_OBJECT_Props,
                              SPA_PARAM_Props);
  spa_pod_builder_prop(&builder, SPA_PROP_params, 0);

  spa_pod_frame struct_frame{};
  spa_pod_builder_push_struct(&builder, &struct_frame);

  for (auto &&[key, value] : parameters) {
    spa_pod_builder_string(&builder, key.c_str());
    if (std::holds_alternative<double>(value)) {
      spa_pod_builder_double(&builder, std::get<double>(value));
    } else if (std::holds_alternative<std::string>(value)) {
      spa_pod_builder_string(&builder, std::get<std::string>(value).c_str());
    } else if (std::holds_alternative<int>(value)) {
      spa_pod_builder_int(&builder, std::get<int>(value));
    } else if (std::holds_alternative<float>(value)) {
      spa_pod_builder_float(&builder, std::get<float>(value));
    } else if (std::holds_alternative<bool>(value)) {
      spa_pod_builder_bool(&builder, std::get<bool>(value));
    } else if (std::holds_alternative<std::nullopt_t>(value)) {
      spa_pod_builder_none(&builder);
    } else {
      throw std::runtime_error("unknown type");
    }
  }

  spa_pod_builder_pop(&builder, &struct_frame);

  return static_cast<spa_pod*>(spa_pod_builder_pop(&builder, &object_frame));
}

void tools::set_props_param(pw_client *client, pw_main_loop *loop,
                            std::span<std::tuple<
                              std::string, param_value_variant>> const
                            parameters) {
  pw_invoke_set_param_data data{.client = client,};
  data.pod = build_set_params_message(data.buffer, 1024, parameters);

  auto func = [](struct spa_loop *loop, bool async, u_int32_t seq,
                 const void *data, size_t size, void *user_data) -> int {
    const auto invoke_data = static_cast<const pw_invoke_set_param_data*>(data);
    pw_node_set_param(reinterpret_cast<pw_node *>(invoke_data->client),
                      SPA_PARAM_Props, 0, invoke_data->pod);
    return 0;
  };

  pw_loop_invoke(pw_main_loop_get_loop(loop), func, 0, &data,
                 sizeof(pw_invoke_set_param_data), true, nullptr);
}
