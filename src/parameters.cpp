#include "parameters/parameters.h"

#include <memory>

std::shared_ptr<parameters::parameter> parameters::none =
    std::make_shared<parameters::parameter>(0, "none", "none", 0.0, 0.0);
std::shared_ptr<parameters::parameter> parameters::saturator_level_in =
    std::make_shared<parameters::parameter>(1, "level_in", "Saturator",
                                            0.015625, 64.0);
std::shared_ptr<parameters::parameter> parameters::saturator_drive =
    std::make_shared<parameters::parameter>(2, "drive", "Saturator", 0.1, 10.0);
std::shared_ptr<parameters::parameter> parameters::saturator_blend =
    std::make_shared<parameters::parameter>(3, "blend", "Saturator", -10, 10);
std::shared_ptr<parameters::parameter> parameters::saturator_level_out =
    std::make_shared<parameters::parameter>(4, "level_out", "Saturator",
                                            0.015625, 64.0);
std::shared_ptr<parameters::parameter> parameters::compressor_threshold =
    std::make_shared<parameters::parameter>(5, "threshold", "Compressor",
                                            0.000977, 1.0);
std::shared_ptr<parameters::parameter> parameters::compressor_ratio =
    std::make_shared<parameters::parameter>(6, "ratio", "Compressor", 1.0,
                                            20.0);
std::shared_ptr<parameters::parameter> parameters::compressor_attack =
    std::make_shared<parameters::parameter>(7, "attack", "Compressor", 0.01,
                                            2000.0);
std::shared_ptr<parameters::parameter> parameters::compressor_release =
    std::make_shared<parameters::parameter>(8, "release", "Compressor", 0.01,
                                            2000.0);
std::shared_ptr<parameters::parameter> parameters::compressor_makeup =
    std::make_shared<parameters::parameter>(9, "makeup", "Compressor", 1.0,
                                            64.0);
std::shared_ptr<parameters::parameter> parameters::compressor_knee =
    std::make_shared<parameters::parameter>(10, "knee", "Compressor", 1.0, 8.0);
std::shared_ptr<parameters::parameter> parameters::compressor_mix =
    std::make_shared<parameters::parameter>(11, "mix", "Compressor", 0.0, 1.0);
std::shared_ptr<parameters::parameter> parameters::equalizer_low =
    std::make_shared<parameters::parameter>(12, "low", "Equalizer", -24, 24);
std::shared_ptr<parameters::parameter> parameters::equalizer_mid =
    std::make_shared<parameters::parameter>(13, "mid", "Equalizer", -24, 24);
std::shared_ptr<parameters::parameter> parameters::equalizer_high =
    std::make_shared<parameters::parameter>(14, "high", "Equalizer", -24, 24);
std::shared_ptr<parameters::parameter> parameters::equalizer_master =
    std::make_shared<parameters::parameter>(15, "master", "Equalizer", -24, 24);
std::shared_ptr<parameters::parameter> parameters::equalizer_low_mid =
    std::make_shared<parameters::parameter>(16, "low_mid", "Equalizer", 0.0,
                                            1000.0);
std::shared_ptr<parameters::parameter> parameters::equalizer_mid_high =
    std::make_shared<parameters::parameter>(17, "mid_high", "Equalizer", 1000.0,
                                            20000.0);
