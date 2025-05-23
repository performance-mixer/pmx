#include <chrono>
#include <future>
#include <iostream>
#include <vector>
#include <bits/ostream.tcc>
#include <pwcpp/filter/app_builder.h>
#include <spa/control/control.h>
#include <spa/pod/builder.h>
#include <spa/pod/iter.h>
#include <spa/pod/pod.h>
#include <systemd/sd-daemon.h>

#include "filter_chain_control/filter_chain_control.h"

int main(const int argc, char **argv) {
  sd_notify(0, "STATUS=Starting");

  pwcpp::filter::AppBuilder<std::nullptr_t> builder;
  builder.set_filter_name("pmx-ctrl-pipe").set_media_type("application/control")
         .set_media_class("application/control").add_arguments(argc, argv).
         add_input_port("lp_mini_in", "32 bit raw UMP").
         add_output_port("lp_mini_out", "32 bit raw UMP").add_signal_processor(
           [](auto position, auto &in_ports, auto &out_ports, auto &user_data,
              auto &parameters) {
             auto out_buffer = out_ports[0]->get_buffer();
             if (out_buffer.has_value() == false) {
               return;
             }

             auto spa_data = out_buffer->get_spa_data(0);
             spa_pod_builder pod_builder{};
             spa_pod_frame frame{};
             spa_pod_builder_init(&pod_builder, spa_data->data,
                                  spa_data->maxsize);
             spa_pod_builder_push_sequence(&pod_builder, &frame, 0);


             auto in_buffer = in_ports.at(0)->get_buffer();
             if (in_buffer.has_value()) {
               auto pod = in_buffer.value().get_pod(0);

               if (pod.has_value()) {
                 if (!spa_pod_is_sequence(pod.value())) {
                   return;
                 }

                 auto sequence = reinterpret_cast<struct spa_pod_sequence*>(pod.
                   value());
                 spa_pod_control *pod_control;
                 SPA_POD_SEQUENCE_FOREACH(sequence, pod_control) {
                   const void *data = SPA_POD_BODY(&pod_control->value);
                   uint32_t length = SPA_POD_BODY_SIZE(&pod_control->value);
                   spa_pod_builder_control(&pod_builder, 0, pod_control->type);
                   spa_pod_builder_bytes(&pod_builder, data, length);
                 }
               }
             }

             [[maybe_unused]] auto pod = static_cast<spa_pod*>(
               spa_pod_builder_pop(&pod_builder, &frame));

             spa_data->chunk->size = pod_builder.state.offset;

             out_buffer->finish();
           });


  auto filter_app = builder.build();
  if (filter_app.has_value()) {
    sd_notify(0, "READY=1");
    filter_app.value()->run();
  }


  sd_notify(0, "READY=1\nSTATUS=Forwarding Control Messages");
}
