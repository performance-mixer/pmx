#include <oscpp/client.hpp>

#include <systemd/sd-daemon.h>

#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <thread>

#include <pwcpp/filter/app_builder.h>

#include <boost/lockfree/spsc_queue.hpp>
#include <logging/logger.h>

struct traktor_z1_message {
  std::uint16_t gain_l;
  std::uint16_t hi_l;
  std::uint16_t mid_l;
  std::uint16_t low_l;
  std::uint16_t filter_l;
  std::uint16_t gain_r;
  std::uint16_t hi_r;
  std::uint16_t mid_r;
  std::uint16_t low_r;
  std::uint16_t filter_r;
  std::uint16_t cue_mix;
  std::uint16_t fader_l;
  std::uint16_t fader_r;
  std::uint16_t cross_fader;
};

int main(int argc, char *argv[]) {
  sd_notify(0, "STATUS=Starting");
  logging::Logger logger{"main"};
  std::string device_path = "/dev/hidraw8";

  if (argc > 1) device_path = argv[1];
  logger.log_info("Using device path: " + device_path, "DEVICE_PATH=",
                  device_path);

  boost::lockfree::spsc_queue<traktor_z1_message> queue(128);

  traktor_z1_message previous{};
  previous.gain_l = 0;
  previous.hi_l = 0;
  previous.mid_l = 0;
  previous.low_l = 0;
  previous.filter_l = 0;
  previous.gain_r = 0;
  previous.hi_r = 0;
  previous.mid_r = 0;
  previous.low_r = 0;
  previous.filter_r = 0;
  previous.cue_mix = 0;
  previous.fader_l = 0;
  previous.fader_r = 0;
  previous.cross_fader = 0;
  bool is_init(true);

  logger.log_info("Building filter app");
  pwcpp::filter::AppBuilder<std::nullptr_t> builder;
  builder.set_filter_name("pmx-traktor-z1-router").set_media_type("Osc").
          set_media_class("Osc/Source").add_arguments(argc, argv).
          add_output_port("pmx-osc", "8 bit raw midi").add_signal_processor(
            [&queue, &previous, &is_init](auto position, auto &in_ports,
                                          auto &out_ports, auto &user_data,
                                          auto &parameters) {
              if (queue.read_available() == 0) return;

              auto buffer = out_ports[0]->get_buffer();
              if (buffer.has_value()) {
                if (queue.read_available() > 0) {
                  traktor_z1_message message{};
                  while (queue.pop(message)) {
                    if (is_init) {
                      previous = message;
                      is_init = false;
                    } else {
                      std::array<
                        std::optional<std::tuple<std::string, std::uint16_t>>,
                        14> changes{};

                      if (previous.gain_l != message.gain_l) {
                        changes[0] = std::make_tuple(
                          "/L/A/MIX/fx_gain", message.gain_l);
                      }

                      if (previous.hi_l != message.hi_l) {
                        changes[1] = std::make_tuple(
                          "/L/A/EQU/high", message.hi_l);
                      }

                      if (previous.mid_l != message.mid_l) {
                        changes[2] = std::make_tuple(
                          "/L/A/EQU/mid", message.mid_l);
                      }

                      if (previous.low_l != message.low_l) {
                        changes[3] = std::make_tuple(
                          "/L/A/EQU/low", message.low_l);
                      }

                      if (previous.filter_l != message.filter_l) {
                        changes[4] = std::make_tuple(
                          "/L/A/MIX/fx_mix", message.filter_l);
                      }

                      if (previous.gain_r != message.gain_r) {
                        changes[5] = std::make_tuple(
                          "/L/B/MIX/fx_gain", message.gain_r);
                      }

                      if (previous.hi_r != message.hi_r) {
                        changes[6] = std::make_tuple(
                          "/L/B/EQU/high", message.hi_r);
                      }

                      if (previous.mid_r != message.mid_r) {
                        changes[7] = std::make_tuple(
                          "/L/B/EQU/mid", message.mid_r);
                      }

                      if (previous.low_r != message.low_r) {
                        changes[8] = std::make_tuple(
                          "/L/B/EQU/low", message.low_r);
                      }

                      if (previous.filter_r != message.filter_r) {
                        changes[9] = std::make_tuple(
                          "/L/B/MIX/fx_mix", message.filter_r);
                      }

                      if (previous.cue_mix != message.cue_mix) {
                        changes[10] = std::make_tuple(
                          "/cue_mix", message.cue_mix);
                      }

                      if (previous.fader_l != message.fader_l) {
                        changes[11] = std::make_tuple(
                          "/L/A/EQU/master", message.fader_l);
                      }

                      if (previous.fader_r != message.fader_r) {
                        changes[12] = std::make_tuple(
                          "/L/B/EQU/master", message.fader_r);
                      }

                      if (previous.cross_fader != message.cross_fader) {
                        changes[13] = std::make_tuple(
                          "/cross_fader", message.cross_fader);
                      }

                      auto spa_data = buffer->get_spa_data(0);
                      if (std::any_of(changes.begin(), changes.end(),
                                      [](const auto &change) {
                                        return change.has_value();
                                      })) {
                        spa_pod_builder builder;
                        spa_pod_frame frame;
                        spa_pod_builder_init(&builder, spa_data->data,
                                             spa_data->maxsize);
                        spa_pod_builder_push_sequence(&builder, &frame, 0);
                        spa_pod_builder_control(&builder, 0, SPA_CONTROL_OSC);

                        char osc_buffer[4096];
                        for (auto &&change : changes | std::views::filter(
                               [](const auto &change) {
                                 return change.has_value();
                               })) {
                          OSCPP::Client::Packet packet(osc_buffer, 4096);

                          auto [path, value] = change.value();
                          packet.openMessage(path.c_str(), 1).float32(value).
                                 closeMessage();
                          const auto size = packet.size();
                          spa_pod_builder_bytes(&builder, osc_buffer, size);
                        }

                        [[maybe_unused]] auto pod = static_cast<spa_pod*>(
                          spa_pod_builder_pop(&builder, &frame));

                        spa_data->chunk->size = builder.state.offset;
                      }

                      previous = message;
                    }
                  }
                }
                buffer.value().finish();
              }
            });

  logger.log_info("Starting usd device reader");
  std::thread device_reader_thread([&device_path, &queue]() {
    logging::Logger logger{"device_reader_thread"};
    std::ifstream file(device_path, std::ios::binary);

    if (!file.is_open()) {
      logger.log_error("Unable to open device", "DEVICE_PATH=", device_path);
      return 1;
    }

    std::array<std::uint8_t, 30> buffer = {0};
    traktor_z1_message report;

    while (file.read(reinterpret_cast<char*>(buffer.data()), buffer.size())) {
      if (buffer[0] == 1) {
        if (file.gcount() == 30) {
          report.gain_l = (buffer[2] << 8) + buffer[1];
          report.hi_l = (buffer[4] << 8) + buffer[3];
          report.mid_l = (buffer[6] << 8) + buffer[5];
          report.low_l = (buffer[8] << 8) + buffer[7];
          report.filter_l = (buffer[10] << 8) + buffer[9];
          report.gain_r = (buffer[12] << 8) + buffer[11];
          report.hi_r = (buffer[14] << 8) + buffer[13];
          report.mid_r = (buffer[16] << 8) + buffer[15];
          report.low_r = (buffer[18] << 8) + buffer[17];
          report.filter_r = (buffer[20] << 8) + buffer[19];
          report.cue_mix = (buffer[22] << 8) + buffer[21];
          report.fader_l = (buffer[24] << 8) + buffer[23];
          report.fader_r = (buffer[26] << 8) + buffer[25];
          report.cross_fader = (buffer[28] << 8) + buffer[27];
          queue.push(report);
        } else {
          logger.log_error("Invalid report size", "SIZE=", file.gcount());
        }
      }
    }

    file.close();
    return 0;
  });

  auto app = builder.build();
  if (app.has_value()) {
    logger.log_info("Starting filter app");
    sd_notify(0, "READY=1");
    app.value()->run();
  }

  return 0;
}
