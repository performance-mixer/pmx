#include <pwcpp/filter/app_builder.h>

#include <spa/debug/pod.h>

int main(int argc, char *argv[]) {
  pwcpp::filter::AppBuilder builder;
  builder.set_filter_name("pmx-midi-router")
      .set_media_type("Midi")
      .set_media_class("Midi/Sink")
      .add_arguments(argc, argv)
      .add_input_port("input channels", "8 bit raw midi")
      .add_input_port("output channels", "8 bit raw midi")
      .add_output_port("osc", "raw osc")
      .add_signal_processor([](auto position, auto in_ports, auto out_ports) {
        for (auto &&port : in_ports) {
          auto buffer = port->get_buffer();
          if (buffer.has_value()) {
            auto pod = buffer.value().get_pod();
            if (pod.has_value()) {
              spa_debug_pod(0, nullptr, pod.value());
            }
            buffer.value().finish();
          }
        }
      });

  auto filter_app = builder.build();
  if (filter_app.has_value()) {
    filter_app.value()->run();
  }
}
