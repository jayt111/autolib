#pragma once

#include <optional>

#include "formula.hpp"
#include "negate.hpp"
#include "omega/core/types.hpp"
#include "translate_to_buchi.hpp"

#include <omega/algorithms/emptiness_buchi.hpp>
#include <omega/kripke/explicit_kripke.hpp>
#include <omega/kripke/product_kripke_buchi.hpp>
#include <vector>

namespace ltl_mc {

struct KripkeLassoWitness {
  std::vector<omega::core::StateId> prefix;
  std::vector<omega::core::StateId> cycle;
};

struct ModelCheckResult {
  bool holds = false;

  // witness if holds is false
  // witness in product automaton
  std::optional<omega::algorithms::BuchiLassoWitness> product_counterexample;
  // projected witness in the kripke structure
  std::optional<KripkeLassoWitness> kripke_counterexample;
};

inline KripkeLassoWitness
project_witness_to_kripke(const omega::kripke::KripkeBuchiProduct &product,
                          const omega::algorithms::BuchiLassoWitness &w) {
  KripkeLassoWitness out;

  out.prefix.reserve(w.prefix.size());
  for (auto s : w.prefix) {
    out.prefix.push_back(product.decode(s).kripke_state);
  }
  out.cycle.reserve(w.cycle.size());
  for (auto s : w.cycle) {
    out.cycle.push_back(product.decode(s).kripke_state);
  }
  return out;
}

inline ModelCheckResult model_check(const omega::kripke::ExplicitKripke &k,
                                    const FormulaPtr &phi) {
  auto neg_phi = negate(phi);
  auto ba = translate_to_buchi(neg_phi);
  auto product = omega::kripke::product_kripke_buchi(k, ba);
  auto res = omega::algorithms::buchi_emptiness(product.automaton, true);

  ModelCheckResult out;
  out.holds = !res.non_empty;

  if (res.non_empty && res.witness.has_value()) {
    out.product_counterexample = res.witness;
    out.kripke_counterexample =
        project_witness_to_kripke(product, *res.witness);
  }
  return out;
}

} // namespace ltl_mc
