#pragma once

#include <cstddef>
#include <omega/core/types.hpp>
#include <omega/logic/guard.hpp>
#include <span>

namespace omega::automata {

template <class TS, class Acc> class OmegaAutomaton {
public:
  using StateId = omega::core::StateId;
  using edge_type = typename TS::edge_type;

  TS ts;
  Acc acc;

  OmegaAutomaton() = default;

  // --------------------- TS Forwarding ---------------------
  std::size_t num_states() const noexcept { return ts.num_states(); }

  std::span<const StateId> initial_states() const noexcept {
    return ts.initial_states();
  }

  std::span<const edge_type> out_edges(StateId s) const noexcept {
    return ts.out_edges(s);
  }

  // ----------------- Acceptance Forwarding -----------------

  // ------Buchi------
  bool is_accepting(StateId s) const noexcept
    requires requires(const Acc &x, StateId t) { x.is_accepting(t); }
  {
    return acc.is_accepting(s);
  }

  void set_accepting(StateId s, bool value = true)
    requires requires(Acc &x, StateId t, bool v) { x.set_accepting(t, v); }
  {
    acc.set_accepting(s, value);
  }

  // ----- Parity -------
  omega::core::Priority priority(StateId s) const noexcept
    requires requires(const Acc &x, StateId t) { x.priority(t); }
  {
    return acc.priority(s);
  }

  void set_priority(StateId s, omega::core::Priority p)
    requires requires(Acc &x, StateId t, omega::core::Priority q) {
      x.set_priority(t, q);
    }
  {
    acc.set_priority(s, p);
  }

  // -- Rabin / Streett --
  std::size_t num_pairs() const noexcept
    requires requires(const Acc &x) { x.num_pairs(); }
  {
    return acc.num_pairs();
  }

  bool in_E(std::size_t i, StateId s) const noexcept
    requires requires(const Acc &x, std::size_t j, StateId t) { x.in_E(j, t); }
  {
    return acc.in_E(i, s);
  }

  bool in_F(std::size_t i, StateId s) const noexcept
    requires requires(const Acc &x, std::size_t j, StateId t) { x.in_F(j, t); }
  {
    return acc.in_F(i, s);
  }

  std::size_t add_pair()
    requires requires(Acc &x) { x.add_pair(); }
  {
    return acc.add_pair();
  }

  void set_E(std::size_t i, StateId s, bool v = true)
    requires requires(Acc &x, std::size_t j, StateId t, bool b) {
      x.set_E(j, t, b);
    }
  {
    acc.set_E(i, s, v);
  }

  void set_F(std::size_t i, StateId s, bool v = true)
    requires requires(Acc &x, std::size_t j, StateId t, bool b) {
      x.set_F(j, t, b);
    }
  {
    acc.set_F(i, s, v);
  }

  // ------ Common -------
  void resize_acceptance(std::size_t n)
    requires requires(Acc &x, std::size_t m) { x.resize(m); }
  {
    acc.resize(n);
  }

  // -------------------- Construction API ----------------------

  StateId add_state() {
    StateId s = ts.add_state();
    if constexpr (requires(Acc &x, std::size_t n) { x.resize(n); }) {
      // Keep acceptance in sync with TS size
      acc.resize(ts.num_states());
    }
    return s;
  }

  // Buchi
  StateId add_state(bool accepting)
    requires requires(Acc &x, StateId t, bool v) { x.set_accepting(t, v); }
  {
    StateId s = add_state();
    acc.set_accepting(s, accepting);
    return s;
  }

  void reserve_states(std::size_t n) { ts.reserve_states(n); }

  void add_initial(StateId s) { ts.add_initial(s); }
  void clear_initials() noexcept { ts.clear_initials(); }

  void add_edge(StateId src, StateId dst) { ts.add_edge(src, dst); }

  void add_edge(StateId src, StateId dst, omega::logic::Guard &g) {
    ts.add_edge(src, dst, g);
  }

  edge_type &add_edge_ref(StateId src, StateId dst) {
    return ts.add_edge_ref(src, dst);
  }
};

} // namespace omega::automata
