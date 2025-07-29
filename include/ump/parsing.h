#pragma once

#include <optional>
#include <expected>

#include "messages.h"
#include "error.h"

namespace ump {
std::expected<messages::message, error> parse(uint32_t first,
                        std::optional<uint32_t> second = std::nullopt);
}
