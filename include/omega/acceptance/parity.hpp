#pragma once

#include <cassert>
#include <cstddef>
#include <omega/core/types.hpp>
#include <vector>

namespace omega::acceptance {

class Parity {
public:
  using StadeId = omega::core::StateId;
  using Priority = omega::core::Priority;

  Parity() = default;

  explicit Parity(std::size_t n_states, Priority default_priority = 0)
      : prio_(n_states, default_priority), max_(default_priority) {}

  void resize(std::size_t n_states, Priority default_priority = 0) {
    prio_.resize(n_states, default_priority);
    recompute_max_();
  }

  std::size_t size() const noexcept { return prio_.size(); }

  Priority priority(StadeId s) const noexcept {
    assert(s.valid());
    assert(s.raw() < prio_.size());
    return prio_[s.raw()];
  }

  void set_priority(StadeId s, Priority p) {
    assert(s.valid());
    assert(s.raw() < prio_.size());
    prio_[s.raw()] = p;
    if (p > max_)
      max_ = p;
  }

  Priority max_priority() const noexcept { return max_; }

private:
  void recompute_max_() noexcept {
    Priority m = 0;
    for (Priority p : prio_)
      if (p > m)
        m = p;
    max_ = m;
  }

  std::vector<Priority> prio_;
  Priority max_ = 0;
};
} // namespace omega::acceptance
