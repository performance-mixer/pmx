#pragma once

#include "events.h"
#include "error/error.h"

#include <expected>
#include <optional>

namespace slp::ctrl::launchpad::mini {
std::expected<session::event, error::error> parse_session_midi(
  unsigned int first, std::optional<unsigned int> second = std::nullopt);
}
