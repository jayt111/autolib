#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <omega/core/types.hpp>
#include <vector>

// NOTE: Both Rabin and Street share the same interface

namespace omega::acceptance {

template <class Tag> class PairAcceptance {
public:
  using StateId = omega::core::StateId;

  PairAcceptance() = default;

  explicit PairAcceptance(std::size_t n_states) { resize(n_states); }

  void resize(std::size_t n_states) {
    n_states_ = n_states;
    for (auto &e : E_)
      e.resize(n_states_, 0);

    for (auto &f : F_)
      f.resize(n_states_, 0);
  }

  std::size_t num_states() const noexcept { return n_states_; }

  std::size_t num_pairs() const noexcept { return E_.size(); }

  // Add new (E,F) pair
  std::size_t add_pair() {
    E_.emplace_back(n_states_, 0);
    F_.emplace_back(n_states_, 0);
    return E_.size() - 1;
  }

  // Membership queries
  bool in_E(std::size_t i, StateId s) const noexcept {
    assert(E_.size());
    assert(s.valid());
    assert(s.raw() < n_states_);
    return E_[i][s.raw()] != 0;
  }

  bool in_F(std::size_t i, StateId s) const noexcept {
    assert(F_.size());
    assert(s.valid());
    assert(s.raw() < n_states_);
    return F_[i][s.raw()] != 0;
  }

  // Mutation helpers
  void set_E(std::size_t i, StateId s, bool value = true) {
    assert(i < E_.size());
    assert(s.valid());
    assert(s.raw() < n_states_);
    E_[i][s.raw()] = value ? 1u : 0u;
  }

  void set_F(std::size_t i, StateId s, bool value = true) {
    assert(i < F_.size());
    assert(s.valid());
    assert(s.raw() < n_states_);
    F_[i][s.raw()] = value ? 1u : 0u;
  }

  void clear_E(std::size_t i) {
    assert(i < E_.size());
    std::fill(E_[i].begin(), E_[i].end(), 0u);
  }

  void clear_F(std::size_t i) {
    assert(i < F_.size());
    std::fill(F_[i].begin(), F_[i].end(), 0u);
  }

private:
  std::size_t n_states_ = 0;
  std::vector<std::vector<std::uint8_t>> E_;
  std::vector<std::vector<std::uint8_t>> F_;
};

} // namespace omega::acceptance
