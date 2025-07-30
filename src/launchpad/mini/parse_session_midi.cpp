#include "slp_ctrl/launchpad/mini/parse_session_midi.h"

std::expected<slp::ctrl::launchpad::mini::session::event, error::error>
slp::ctrl::launchpad::mini::parse_session_midi(unsigned int first,
                                               std::optional<unsigned int>
                                               second) {
  return std::unexpected(error::error::not_implemented());
}
