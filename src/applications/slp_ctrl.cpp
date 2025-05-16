#include <systemd/sd-daemon.h>

#include <logging/logger.h>
#include <pwcpp/filter/app_builder.h>

int main(const int argc, char *argv[]) {
  sd_notify(0, "STATUS=Starting");
  logging::Logger logger{"main"};

  pwcpp::filter::AppBuilder<std::nullptr_t> builder;
  builder.set_filter_name("pmx-slp-ctrl").set_media_type("application/control").
          set_media_class("application/control").add_arguments(argc, argv).
          add_input_port("pmx-from-sl-osc", "8 bit raw control").
          add_input_port("lp_mini", "32 bit raw UMP").
          add_output_port("pmx-to-sl-osc", "8 bit raw control").
          add_output_port("lp_mini", "32 bit raw UMP").
          add_signal_processor([](auto position, auto &in_ports,
                                  auto &out_ports, auto &user_data,
                                  auto &parameters) {
            auto sl_osc_out_buffer = out_ports[0]->get_buffer();
            auto lp_mini_buffer = out_ports[1]->get_buffer();
            if (sl_osc_out_buffer.has_value() && lp_mini_buffer.has_value()) {
              logging::Logger logger{"signal-processor"};
              auto spa_data = sl_osc_out_buffer->get_spa_data(0);
              spa_pod_builder builder{};
              spa_pod_frame frame{};
              spa_pod_builder_init(&builder, spa_data->data, spa_data->maxsize);
              spa_pod_builder_push_sequence(&builder, &frame, 0);

              auto sl_osc_input_buffer = in_ports[0]->get_buffer();
              auto lp_mini_input_buffer = in_ports[1]->get_buffer();

              [[maybe_unused]] auto pod = static_cast<spa_pod*>(
                spa_pod_builder_pop(&builder, &frame));

              spa_data->chunk->size = builder.state.offset;
              sl_osc_out_buffer.value().finish();
            }
          });

  auto filter_app = builder.build();
  if (filter_app.has_value()) {
    sd_notify(0, "READY=1");
    logger.log_info("Starting filter app");
    filter_app.value()->run();
  }

  return 0;
}
