#pragma once

#include <cstddef>
#include <omega/automata/explicit_buchi.hpp>
#include <omega/core/types.hpp>
#include <omega/kripke/explicit_kripke.hpp>
#include <omega/logic/guard_eval.hpp>
#include <vector>

namespace omega::kripke {

struct KripkeBuchiProductState {
  omega::core::StateId kripke_state;
  omega::core::StateId buchi_state;
};

struct KripkeBuchiProduct {
  omega::automata::ExplicitBuchiAutomata automaton;
  std::vector<KripkeBuchiProductState> state_info;

  const KripkeBuchiProductState &decode(omega::core::StateId s) const {
    return state_info[s.raw()];
  }
};

inline KripkeBuchiProduct
product_kripke_buchi(const omega::kripke::ExplicitKripke &k,
                     const omega::automata::ExplicitBuchiAutomata &a) {

  using StateId = omega::core::StateId;

  KripkeBuchiProduct out;

  const std::size_t nk = k.num_states();
  const std::size_t na = a.num_states();

  auto id_of = [&](StateId ks, StateId as) -> StateId {
    const std::size_t idx = ks.raw() * na + as.raw();
    return StateId{static_cast<StateId::underlying_type>(idx)};
  };

  out.automaton.reserve_states(nk * na);

  for (std::size_t ik = 0; ik < nk; ++ik) {
    StateId ks{static_cast<StateId::underlying_type>(ik)};
    for (std::size_t ia = 0; ia < na; ++ia) {
      StateId as{static_cast<StateId::underlying_type>(ia)};
      out.automaton.add_state(a.is_accepting(as));
      out.state_info.push_back({ks, as});
    }
  }

  // ------ Intials ------
  for (StateId ks0 : k.initial_states()) {
    for (StateId as0 : a.initial_states()) {
      out.automaton.add_initial(id_of(ks0, as0));
    }
  }

  // ---- Transitions ----
  for (std::size_t ik = 0; ik < nk; ++ik) {
    StateId ks{static_cast<StateId::underlying_type>(ik)};

    for (std::size_t ia = 0; ia < na; ++ia) {
      StateId as{static_cast<StateId::underlying_type>(ia)};
      StateId src = id_of(ks, as);

      for (const auto &ke : k.out_edges(ks)) {
        StateId ks_next = ke.dst;

        for (const auto &ae : a.out_edges(as)) {
          bool enabled = true;

          if (ae.guard.has_value()) {
            enabled = omega::logic::guard_holds(*ae.guard, k, ks);
          }

          if (!enabled) {
            continue;
          }

          out.automaton.add_edge(src, id_of(ks_next, ae.dst));
        }
      }
    }
  }
  return out;
}

} // namespace omega::kripke
