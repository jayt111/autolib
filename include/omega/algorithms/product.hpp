#pragma once

#include <cstddef>
#include <omega/automata/explicit_buchi.hpp>
#include <omega/core/concepts.hpp>
#include <omega/core/types.hpp>

namespace omega::algorithms {

template <omega::core::BuchiAutomaton A, omega::core::BuchiAutomaton B>
automata::ExplicitBuchiAutomata product_buchi(const A &a, const B &b) {
  using StateId = omega::core::StateId;

  // Product automaton
  automata::ExplicitBuchiAutomata p;

  // Num states
  const std::size_t na = a.num_states();
  const std::size_t nb = b.num_states();

  // Lambda functions
  auto idx_of = [&](StateId qa, StateId qb, std::size_t phase) -> std::size_t {
    return ((phase * na + qa.raw()) * nb + qb.raw());
  };
  auto id_of = [&](StateId qa, StateId qb, std::size_t phase) -> StateId {
    return StateId{
        static_cast<StateId::underlying_type>(idx_of(qa, qb, phase))};
  };

  p.reserve_states(2 * na * nb);

  for (std::size_t phase = 0; phase < 2; phase++) {
    for (std::size_t ia = 0; ia < na; ia++) {
      for (std::size_t ib = 0; ib < nb; ib++) {
        StateId qb{static_cast<StateId::underlying_type>(ib)};
        const bool accepting = (phase == 1) && b.is_accepting(qb);
        p.add_state(accepting);
      }
    }
  }

  for (StateId ia : a.initial_states()) {
    for (StateId ib : b.initial_states()) {
      p.add_initial(id_of(ia, ib, 0));
    }
  }

  for (std::size_t phase = 0; phase < 2; phase++) {
    for (std::size_t ia = 0; ia < na; ia++) {
      StateId qa{static_cast<StateId::underlying_type>(ia)};

      for (std::size_t ib = 0; ib < nb; ib++) {
        StateId qb{static_cast<StateId::underlying_type>(ib)};
        StateId src = id_of(qa, qb, phase);

        std::size_t next_phase = phase;

        if (phase == 0 && a.is_accepting(qa)) {
          next_phase = 1;
        } else if (phase == 1 && b.is_accepting(qb)) {
          next_phase = 0;
        }

        for (const auto &ea : a.out_edges(qa)) {
          for (const auto &eb : b.out_edges(qb)) {
            p.add_edge(src, id_of(ea.dst, eb.dst, next_phase));
          }
        }
      }
    }
  }
  return p;
}
} // namespace omega::algorithms
