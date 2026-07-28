#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <omega/core/types.hpp>
#include <vector>

namespace omega::acceptance {

class Buchi {
public:
  using StateId = omega::core::StateId;

  Buchi() = default;

  explicit Buchi(std::size_t n_States) : accepting_(n_States, 0) {}

  void resize(std::size_t n_states) { accepting_.resize(n_states, 0); }

  bool is_accepting(StateId s) const noexcept {
    assert(s.valid());
    assert(s.raw() < accepting_.size());
    return accepting_[s.raw()] != 0;
  }

  void set_accepting(StateId s, bool value = true) {
    assert(s.valid());
    assert(s.raw() < accepting_.size());
    accepting_[s.raw()] = value ? 1u : 0u;
  }

private:
  std::vector<std::uint8_t> accepting_;
};

} // namespace omega::acceptance
