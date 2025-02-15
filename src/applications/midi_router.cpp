#include "interpolation/interpolation.h"
#include "parameters/parameters.h"
#include "midi/router/midi_router.h"

#include <systemd/sd-daemon.h>

#include <cstddef>
#include <iostream>
#include <ranges>
#include <logging/logger.h>

#include <oscpp/client.hpp>

#include <pwcpp/filter/app_builder.h>
#include <pwcpp/midi/control_change.h>
#include <pwcpp/midi/message.h>
#include <pwcpp/midi/parse_midi.h>

#include <spa/pod/builder.h>

std::string build_input_channel_osc_path(
  const pwcpp::midi::control_change &message,
  const parameters::parameter &parameter) {
  std::stringstream osc_path;

  osc_path << std::format("/I/A/{}/{}/{}",
                          static_cast<int>(message.channel) + 1,
                          parameter.device_short_name, parameter.name);

  return osc_path.str();
}

int main(const int argc, char *argv[]) {
  sd_notify(0, "STATUS=Starting");
  logging::Logger logger{"main"};

  pwcpp::filter::AppBuilder<std::nullptr_t> builder;
  logger.log_info("Building filter app");
  builder.set_filter_name("pmx-midi-router").set_media_type("Midi").
          set_media_class("Midi/Sink").add_arguments(argc, argv).
          add_input_port("input channels", "8 bit raw midi").
          add_input_port("output channels", "8 bit raw midi").
          add_output_port("osc", "8 bit raw midi").add_signal_processor(
            [](auto position, auto &in_ports, auto &out_ports, auto &user_data,
               auto &parameters) {
              auto out_buffer = out_ports[0]->get_buffer();
              if (out_buffer.has_value() == false) {
                return;
              }

              auto spa_data = out_buffer->get_spa_data(0);
              spa_pod_builder builder{};
              spa_pod_frame frame{};
              spa_pod_builder_init(&builder, spa_data->data, spa_data->maxsize);
              spa_pod_builder_push_sequence(&builder, &frame, 0);

              logging::Logger logger{"signal_processor"};
              auto input_channels_buffer = in_ports.at(0)->get_buffer();

              if (input_channels_buffer.has_value()) {
                midi::router::process_input_channels_port(
                  logger, input_channels_buffer.value(), builder);
              }

              [[maybe_unused]] auto pod = static_cast<spa_pod*>(spa_pod_builder_pop(
                &builder, &frame));

              spa_data->chunk->size = builder.state.offset;
              out_buffer->finish();
            });

  auto filter_app = builder.build();
  if (filter_app.has_value()) {
    logger.log_info("Starting filter app");
    sd_notify(0, "READY=1");
    filter_app.value()->run();
  }
}
