#pragma once

#include <omega/algorithms/emptiness_parity.hpp>
#include <omega/algorithms/rabin_to_parity.hpp>
#include <omega/core/concepts.hpp>

namespace omega::algorithms {

using RabinEmptinessResult = ParityEmptinessResult;

template <omega::core::RabinAutomaton A>
RabinEmptinessResult rabin_emptiness(const A &a, bool want_witness = false) {
  auto p = rabin_to_parity(a);
  return parity_emptiness(p, want_witness);
}

} // namespace omega::algorithms
