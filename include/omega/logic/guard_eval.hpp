#pragma once

#include "omega/core/types.hpp"
#include <omega/kripke/explicit_kripke.hpp>
#include <omega/logic/guard.hpp>

namespace omega::logic {

inline bool guard_holds(const Guard &g, const omega::kripke::ExplicitKripke &k,
                        omega::core::StateId s) {
  for (auto ap : g.positive()) {
    if (!k.holds(s, ap)) {
      return false;
    }
  }
  for (auto ap : g.negative()) {
    if (k.holds(s, ap)) {
      return false;
    }
  }
  return true;
}
} // namespace omega::logic
