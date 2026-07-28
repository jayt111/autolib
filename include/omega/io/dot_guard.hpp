#pragma once

#include <ostream>

#include <omega/logic/guard.hpp>

namespace omega::io {

inline void write_guard_label(std::ostream &os, const omega::logic::Guard &g) {
  bool first = true;

  for (auto ap : g.positive()) {
    if (!first)
      os << " && ";
    os << "p" << ap.raw();
    first = false;
  }

  for (auto ap : g.negative()) {
    if (!first)
      os << " && ";
    os << "!p" << ap.raw();
    first = false;
  }

  if (first) {
    os << "true";
  }
}

} // namespace omega::io
