#pragma once

#include <cstddef>
#include <cstdint>
#include <omega/core/types.hpp>
#include <omega/ts/explicit_ts.hpp>
#include <span>
#include <vector>

// Basically transition system with labels over atomic propositons

namespace omega::kripke {
class ExplicitKripke {
public:
  using StateId = omega::core::StateId;
  using APId = omega::core::APId;
  using edge_type = omega::ts::ExplicitTS::edge_type;

  ExplicitKripke() = default;

  // --- TS Forwarding ---
  std::size_t num_states() const noexcept { return ts_.num_states(); }
  std::span<const StateId> initial_states() const noexcept {
    return ts_.initial_states();
  }
  std::span<const edge_type> out_edges(StateId s) const noexcept {
    return ts_.out_edges(s);
  }
  void add_initial(StateId s) { ts_.add_initial(s); }
  void clear_initials() noexcept { ts_.clear_initials(); }
  void add_edge(StateId src, StateId dst) { ts_.add_edge(src, dst); }
  edge_type &add_edge_ref(StateId src, StateId dst) {
    return ts_.add_edge_ref(src, dst);
  }

  // -------------------- Kripke Specific --------------------
  StateId add_state() {
    StateId s = ts_.add_state();
    labeling_.emplace_back(num_aps_, 0u);
    return s;
  }

  void reserve_states(std::size_t n) {
    ts_.reserve_states(n);
    labeling_.reserve(n);
  }

  std::size_t num_atomic_propositions() const noexcept { return num_aps_; }

  bool holds(StateId s, APId ap) const noexcept {
    assert_state(s);
    assert_ap(ap);
    return labeling_[s.raw()][ap.raw()] != 0;
  }

  std::span<const std::uint8_t> valuation(StateId s) const noexcept {
    assert_state(s);
    const auto &row = labeling_[s.raw()];
    return std::span<const std::uint8_t>(row.data(), row.size());
  }

  // -------- AP ---------
  APId add_atomic_proposition() {
    const std::size_t idx = num_aps_;
    assert(idx < APId::invalid_value());
    num_aps_++;
    for (auto &row : labeling_) {
      row.push_back(0u);
    }
    return APId{static_cast<APId::underlying_type>(idx)};
  }

  void reserve_atomic_propositions(std::size_t n) {
    for (auto &row : labeling_) {
      row.reserve(n);
    }
  }

  // ----- Labeling ------
  void set_holds(StateId s, APId ap, bool value = true) {
    assert_state(s);
    assert_ap(ap);
    labeling_[s.raw()][ap.raw()] = value ? 1u : 0u;
  }

  void clear_labeling(StateId s) {
    assert_state(s);
    auto &row = labeling_[s.raw()];
    for (auto &bit : row) {
      bit = 0u;
    }
  }

private:
  void assert_state(StateId s) const noexcept {
    assert(s.valid());
    assert(s.raw() < ts_.num_states());
  }

  void assert_ap(APId ap) const noexcept {
    assert(ap.valid());
    assert(ap.raw() < num_aps_);
  }
  omega::ts::ExplicitTS ts_;
  std::size_t num_aps_ = 0;
  std::vector<std::vector<std::uint8_t>> labeling_;
};
} // namespace omega::kripke
