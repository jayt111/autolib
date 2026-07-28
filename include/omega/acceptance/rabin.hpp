#pragma once

#include <omega/acceptance/pairs.hpp>

namespace omega::acceptance {

struct rabin_tag {};

using Rabin = PairAcceptance<rabin_tag>;

} // namespace omega::acceptance
