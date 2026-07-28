#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <unordered_set>
#include <vector>

#include <omega/algorithms/emptiness_buchi.hpp>
#include <omega/core/concepts.hpp>
#include <omega/core/types.hpp>
#include <omega/io/dot_guard.hpp>

namespace omega::io {

namespace detail {

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

template <class A>
inline void write_state(std::ostream &os, const A &a, std::size_t i,
                        bool highlight_cycle_entry = false) {
  using omega::core::StateId;
  StateId s{static_cast<StateId::underlying_type>(i)};

  os << "  s" << i << " [label=\"s" << i;

  if constexpr (omega::core::ParityAutomaton<A>) {
    os << "\\nprio=" << a.priority(s);
  }

  os << "\"";

  if constexpr (omega::core::BuchiAutomaton<A>) {
    if (a.is_accepting(s))
      os << ", shape=doublecircle";
    else
      os << ", shape=circle";
  } else {
    os << ", shape=circle";
  }

  if (highlight_cycle_entry) {
    os << ", penwidth=2";
  }

  os << "];\n";
}

} // namespace detail

template <omega::core::OmegaAutomaton A>
void write_dot(std::ostream &os, const A &a,
               const std::string &graph_name = "automaton") {
  using omega::core::StateId;

  os << "digraph " << graph_name << " {\n";
  os << "  rankdir=LR;\n";
  os << "  __init [shape=point,label=\"\"];\n";

  const std::size_t n = a.num_states();

  for (std::size_t i = 0; i < n; ++i) {
    detail::write_state(os, a, i);
  }

  for (StateId init : a.initial_states()) {
    if (!init.valid() || init.raw() >= n)
      continue;
    os << "  __init -> s" << init.raw() << ";\n";
  }

  for (std::size_t i = 0; i < n; ++i) {
    StateId u{static_cast<StateId::underlying_type>(i)};
    for (const auto &e : a.out_edges(u)) {
      const auto v = e.dst;
      if (!v.valid() || v.raw() >= n)
        continue;

      os << "  s" << u.raw() << " -> s" << v.raw();

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

template <omega::core::OmegaAutomaton A>
void write_dot(std::ostream &os, const A &a,
               const omega::algorithms::BuchiLassoWitness &w,
               const std::string &graph_name = "automaton") {
  using omega::core::StateId;

  const std::size_t n = a.num_states();
  const auto prefix_edges = detail::make_edge_set(w.prefix);
  const auto cycle_edges = detail::make_edge_set(w.cycle);

  os << "digraph " << graph_name << " {\n";
  os << "  rankdir=LR;\n";
  os << "  __init [shape=point,label=\"\"];\n";

  for (std::size_t i = 0; i < n; ++i) {
    StateId s{static_cast<StateId::underlying_type>(i)};
    const bool highlight = (!w.cycle.empty() && s == w.cycle.front());
    detail::write_state(os, a, i, highlight);
  }

  for (StateId init : a.initial_states()) {
    if (!init.valid() || init.raw() >= n)
      continue;
    os << "  __init -> s" << init.raw() << ";\n";
  }

  for (std::size_t i = 0; i < n; ++i) {
    StateId u{static_cast<StateId::underlying_type>(i)};
    for (const auto &e : a.out_edges(u)) {
      const StateId v = e.dst;
      if (!v.valid() || v.raw() >= n)
        continue;

      const auto key = detail::edge_key(static_cast<std::uint32_t>(u.raw()),
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

      if (is_prefix || is_cycle) {
        if (!has_attrs) {
          os << " [";
          has_attrs = true;
        }

        if (is_cycle) {
          if (!first_attr)
            os << ",";
          os << "penwidth=3";
          first_attr = false;
        }

        if (is_prefix) {
          if (!first_attr)
            os << ",";
          os << "penwidth=2,style=dashed";
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
