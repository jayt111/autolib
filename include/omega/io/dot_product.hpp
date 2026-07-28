#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <omega/algorithms/emptiness_buchi.hpp>
#include <omega/io/dot_guard.hpp>
#include <omega/kripke/product_kripke_buchi.hpp>

namespace omega::io {

namespace product_detail {

inline std::uint64_t edge_key(std::uint32_t u, std::uint32_t v) noexcept {
  return (static_cast<std::uint64_t>(u) << 32) | static_cast<std::uint64_t>(v);
}

inline std::unordered_set<std::uint64_t>
make_edge_set(const std::vector<omega::core::StateId> &path) {
  std::unordered_set<std::uint64_t> s;
  if (path.size() < 2)
    return s;

  s.reserve(path.size() - 1);
  for (std::size_t i = 0; i + 1 < path.size(); ++i) {
    const auto u = static_cast<std::uint32_t>(path[i].raw());
    const auto v = static_cast<std::uint32_t>(path[i + 1].raw());
    s.insert(edge_key(u, v));
  }
  return s;
}

inline const char *layer_fill(std::uint32_t q) noexcept {
  // alternate colors by Büchi layer
  return (q % 2 == 0) ? "lightblue" : "lightyellow";
}

} // namespace product_detail

inline void write_dot(std::ostream &os,
                      const omega::kripke::KripkeBuchiProduct &p,
                      const std::string &graph_name = "kripke_buchi_product") {
  using omega::core::StateId;

  const auto &a = p.automaton;

  os << "digraph " << graph_name << " {\n";
  os << "  rankdir=LR;\n";
  os << "  __init [shape=point,label=\"\"];\n";

  // group nodes by Büchi state for nicer layout
  std::unordered_map<std::uint32_t, std::vector<std::size_t>> layers;

  for (std::size_t i = 0; i < a.num_states(); ++i) {
    StateId s{static_cast<StateId::underlying_type>(i)};
    const auto &info = p.decode(s);

    layers[static_cast<std::uint32_t>(info.buchi_state.raw())].push_back(i);

    os << "  s" << i << " [label=\""
       << "(k=" << info.kripke_state.raw() << ", q=" << info.buchi_state.raw()
       << ")\"";

    if (a.is_accepting(s)) {
      os << ", shape=doublecircle";
    } else {
      os << ", shape=circle";
    }

    os << ", style=filled, fillcolor=\""
       << product_detail::layer_fill(info.buchi_state.raw()) << "\"";
    os << "];\n";
  }

  // same-rank groups by Büchi layer
  for (const auto &[q, states] : layers) {
    (void)q;
    os << "  { rank=same; ";
    for (auto i : states) {
      os << "s" << i << "; ";
    }
    os << "}\n";
  }

  for (StateId init : a.initial_states()) {
    os << "  __init -> s" << init.raw() << ";\n";
  }

  for (std::size_t i = 0; i < a.num_states(); ++i) {
    StateId s{static_cast<StateId::underlying_type>(i)};
    for (const auto &e : a.out_edges(s)) {
      os << "  s" << s.raw() << " -> s" << e.dst.raw();

      if (e.guard.has_value()) {
        os << " [label=\"";
        write_guard_label(os, *e.guard);
        os << "\"]";
      }

      os << ";\n";
    }
  }

  os << "}\n";
}

inline void write_dot(std::ostream &os,
                      const omega::kripke::KripkeBuchiProduct &p,
                      const omega::algorithms::BuchiLassoWitness &w,
                      const std::string &graph_name = "kripke_buchi_product") {
  using omega::core::StateId;

  const auto &a = p.automaton;
  const auto prefix_edges = product_detail::make_edge_set(w.prefix);
  const auto cycle_edges = product_detail::make_edge_set(w.cycle);

  os << "digraph " << graph_name << " {\n";
  os << "  rankdir=LR;\n";
  os << "  __init [shape=point,label=\"\"];\n";

  std::unordered_map<std::uint32_t, std::vector<std::size_t>> layers;

  for (std::size_t i = 0; i < a.num_states(); ++i) {
    StateId s{static_cast<StateId::underlying_type>(i)};
    const auto &info = p.decode(s);

    layers[static_cast<std::uint32_t>(info.buchi_state.raw())].push_back(i);

    os << "  s" << i << " [label=\""
       << "(k=" << info.kripke_state.raw() << ", q=" << info.buchi_state.raw()
       << ")\"";

    if (a.is_accepting(s)) {
      os << ", shape=doublecircle";
    } else {
      os << ", shape=circle";
    }

    os << ", style=filled, fillcolor=\""
       << product_detail::layer_fill(info.buchi_state.raw()) << "\"";

    if (!w.cycle.empty() && s == w.cycle.front()) {
      os << ", penwidth=3";
    }

    os << "];\n";
  }

  for (const auto &[q, states] : layers) {
    (void)q;
    os << "  { rank=same; ";
    for (auto i : states) {
      os << "s" << i << "; ";
    }
    os << "}\n";
  }

  for (StateId init : a.initial_states()) {
    os << "  __init -> s" << init.raw() << ";\n";
  }

  for (std::size_t i = 0; i < a.num_states(); ++i) {
    StateId u{static_cast<StateId::underlying_type>(i)};
    for (const auto &e : a.out_edges(u)) {
      StateId v = e.dst;

      const auto key =
          product_detail::edge_key(static_cast<std::uint32_t>(u.raw()),
                                   static_cast<std::uint32_t>(v.raw()));

      const bool is_prefix = prefix_edges.find(key) != prefix_edges.end();
      const bool is_cycle = cycle_edges.find(key) != cycle_edges.end();

      os << "  s" << u.raw() << " -> s" << v.raw();

      bool has_attrs = false;
      bool first_attr = true;

      if (e.guard.has_value()) {
        os << " [label=\"";
        write_guard_label(os, *e.guard);
        os << "\"";
        has_attrs = true;
        first_attr = false;
      }

      if (is_cycle || is_prefix) {
        if (!has_attrs) {
          os << " [";
          has_attrs = true;
        }

        if (is_cycle) {
          if (!first_attr)
            os << ",";
          os << "color=red,penwidth=3";
          first_attr = false;
        }

        if (is_prefix) {
          if (!first_attr)
            os << ",";
          os << "color=blue,penwidth=2,style=dashed";
          first_attr = false;
        }
      }

      if (has_attrs) {
        os << "]";
      }

      os << ";\n";
    }
  }

  os << "}\n";
}

} // namespace omega::io
