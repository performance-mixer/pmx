#pragma once

#include <memory>
#include <string>
#include <vector>
namespace parameters {

struct parameter {
  unsigned int id;
  std::string name;
  std::string device_name;
  double min;
  double max;
  std::string full_name;
  std::string device_short_name;

  parameter(unsigned int id, std::string name, std::string device_name,
            double min, double max, std::string device_short_name)
      : id(id), name(name), device_name(device_name), min(min), max(max),
        full_name(device_name + ":" + name),
        device_short_name(device_short_name) {}

  bool operator==(const parameters::parameter &other) const {
    return this->id == other.id;
  }
};

extern std::shared_ptr<parameter> none;
extern std::shared_ptr<parameter> saturator_level_in;
extern std::shared_ptr<parameter> saturator_drive;
extern std::shared_ptr<parameter> saturator_blend;
extern std::shared_ptr<parameter> saturator_level_out;
extern std::shared_ptr<parameter> compressor_threshold;
extern std::shared_ptr<parameter> compressor_ratio;
extern std::shared_ptr<parameter> compressor_attack;
extern std::shared_ptr<parameter> compressor_release;
extern std::shared_ptr<parameter> compressor_makeup;
extern std::shared_ptr<parameter> compressor_knee;
extern std::shared_ptr<parameter> compressor_mix;
extern std::shared_ptr<parameter> equalizer_low;
extern std::shared_ptr<parameter> equalizer_mid;
extern std::shared_ptr<parameter> equalizer_high;
extern std::shared_ptr<parameter> equalizer_master;
extern std::shared_ptr<parameter> equalizer_low_mid;
extern std::shared_ptr<parameter> equalizer_mid_high;

extern std::vector<std::shared_ptr<parameters::parameter>> all;

inline std::shared_ptr<struct parameter>
find_target_parameter(std::uint8_t cc_number) {
  if (cc_number == 0) {
    return none;
  } else if (cc_number == 1) {
    return saturator_level_in;
  } else if (cc_number == 2) {
    return saturator_drive;
  } else if (cc_number == 3) {
    return saturator_blend;
  } else if (cc_number == 4) {
    return saturator_level_out;
  } else if (cc_number == 5) {
    return compressor_threshold;
  } else if (cc_number == 6) {
    return compressor_ratio;
  } else if (cc_number == 7) {
    return compressor_attack;
  } else if (cc_number == 8) {
    return compressor_release;
  } else if (cc_number == 9) {
    return compressor_makeup;
  } else if (cc_number == 10) {
    return compressor_knee;
  } else if (cc_number == 11) {
    return compressor_mix;
  } else if (cc_number == 12) {
    return none;
  } else if (cc_number == 13) {
    return equalizer_low;
  } else if (cc_number == 14) {
    return equalizer_mid;
  } else if (cc_number == 15) {
    return equalizer_high;
  } else if (cc_number == 16) {
    return equalizer_master;
  } else if (cc_number == 17) {
    return equalizer_low_mid;
  } else if (cc_number == 18) {
    return equalizer_mid_high;
  } else {
    return none;
  }
}

} // namespace parameters
