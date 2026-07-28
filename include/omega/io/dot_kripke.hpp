#pragma once

#include <cstddef>
#include <ostream>
#include <string>

#include <omega/core/types.hpp>
#include <omega/kripke/explicit_kripke.hpp>

namespace omega::io {

inline void write_dot(std::ostream &os, const omega::kripke::ExplicitKripke &k,
                      const std::string &graph_name = "kripke") {
  using omega::core::APId;
  using omega::core::StateId;

  os << "digraph " << graph_name << " {\n";
  os << "  rankdir=LR;\n";
  os << "  __init [shape=point,label=\"\"];\n";

  const std::size_t n = k.num_states();
  const std::size_t m = k.num_atomic_propositions();

  for (std::size_t i = 0; i < n; ++i) {
    StateId s{static_cast<StateId::underlying_type>(i)};

    os << "  s" << i << " [label=\"s" << i;

    bool first = true;
    for (std::size_t j = 0; j < m; ++j) {
      APId ap{static_cast<APId::underlying_type>(j)};
      if (k.holds(s, ap)) {
        if (first) {
          os << "\\n{";
          first = false;
        } else {
          os << ",";
        }
        os << "p" << j;
      }
    }
    if (!first) {
      os << "}";
    }

    os << "\", shape=circle];\n";
  }

  for (StateId init : k.initial_states()) {
    os << "  __init -> s" << init.raw() << ";\n";
  }

  for (std::size_t i = 0; i < n; ++i) {
    StateId s{static_cast<StateId::underlying_type>(i)};
    for (const auto &e : k.out_edges(s)) {
      os << "  s" << s.raw() << " -> s" << e.dst.raw() << ";\n";
    }
  }

  os << "}\n";
}

} // namespace omega::io
