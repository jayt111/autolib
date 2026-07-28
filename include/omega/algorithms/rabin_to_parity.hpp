#pragma once

#include <cstddef>
#include <omega/automata/explicit_parity.hpp>
#include <omega/core/concepts.hpp>
#include <omega/core/types.hpp>
#include <vector>

namespace omega::algorithms {

// Nondet Rabin to Nondet Parity
template <omega::core::RabinAutomaton A>
omega::automata::ExplicitParityAutomaton rabin_to_parity(const A &a) {
  using omega::core::Priority;
  using omega::core::StateId;

  omega::automata::ExplicitParityAutomaton p;

  const std::size_t n = a.num_states();
  const std::size_t k = a.num_pairs();

  // --- Special Case ----
  // No pairs
  // Rabin accepts nothing
  // Return automaton with same graph shape but all priorites odd
  if (k == 0) {
    p.reserve_states(n);
    for (std::size_t i = 0; i < n; i++) {
      p.add_state();
    }
    for (auto init : a.initial_states()) {
      p.add_initial(init);
    }
    for (std::size_t i = 0; i < n; i++) {
      StateId u{static_cast<StateId::underlying_type>(i)};
      for (const auto &e : a.out_edges(u)) {
        p.add_edge(u, e.dst);
      }
    }
    for (std::size_t i = 0; i < n; i++) {
      StateId s{static_cast<StateId::underlying_type>(i)};
      p.set_priority(s, Priority{1});
    }
    return p;
  } // special case

  // map[pair_i][qi] -> StateId of (q, pair_i)
  std::vector<std::vector<StateId>> map(
      k, std::vector<StateId>(n, StateId::invalid()));

  p.reserve_states(n * k);

  for (std::size_t pair_i = 0; pair_i < k; pair_i++) {
    for (std::size_t qi = 0; qi < n; qi++) {
      StateId q{static_cast<StateId::underlying_type>(qi)};
      StateId new_id = p.add_state();
      map[pair_i][qi] = new_id;

      Priority pr;

      if (a.in_E(pair_i, q)) {
        pr = Priority{1};
      } else if (a.in_F(pair_i, q)) {
        pr = Priority{2};
      } else {
        pr = Priority{3};
      }
      p.set_priority(new_id, pr);
    }
  }

  // Initial States
  for (StateId init : a.initial_states()) {
    if (!init.valid() || init.raw() >= n)
      continue;
    for (std::size_t pair_i = 0; pair_i < k; pair_i++) {
      p.add_initial(map[pair_i][init.raw()]);
    }
  }

  // Edges
  for (std::size_t pair_i = 0; pair_i < k; pair_i++) {
    for (std::size_t qi = 0; qi < n; qi++) {
      StateId q{static_cast<StateId::underlying_type>(qi)};
      StateId u = map[pair_i][qi];

      for (const auto &e : a.out_edges(q)) {
        if (!e.dst.valid() || e.dst.raw() >= n)
          continue;
        p.add_edge(u, map[pair_i][e.dst.raw()]);
      }
    }
  }

  return p;
}
} // namespace omega::algorithms
