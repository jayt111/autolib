#include "formula.hpp"
#include "translate_to_buchi.hpp"

#include <omega/algorithms/emptiness_buchi.hpp>
#include <omega/kripke/explicit_kripke.hpp>
#include <omega/kripke/product_kripke_buchi.hpp>

#include <cassert>

using omega::algorithms::buchi_emptiness;
using omega::core::APId;
using omega::kripke::ExplicitKripke;
using omega::kripke::product_kripke_buchi;

using ltl_mc::Formula;
using ltl_mc::translate_to_buchi;

static void test_translate_G_p_accepts_when_p_always_true() {
  ExplicitKripke k;
  auto p = k.add_atomic_proposition();

  auto s0 = k.add_state();
  k.add_initial(s0);
  k.add_edge(s0, s0);
  k.set_holds(s0, p, true);

  auto f = Formula::make_globally(Formula::make_atom(p));
  auto ba = translate_to_buchi(f);

  auto prod = product_kripke_buchi(k, ba);
  auto res = buchi_emptiness(prod.automaton, true);

  assert(res.non_empty);
  assert(res.witness.has_value());
}

static void test_translate_G_p_rejects_when_p_false() {
  ExplicitKripke k;
  auto p = k.add_atomic_proposition();

  auto s0 = k.add_state();
  k.add_initial(s0);
  k.add_edge(s0, s0);
  k.set_holds(s0, p, false);

  auto f = Formula::make_globally(Formula::make_atom(p));
  auto ba = translate_to_buchi(f);

  auto prod = product_kripke_buchi(k, ba);
  auto res = buchi_emptiness(prod.automaton);

  assert(!res.non_empty);
}

static void test_translate_F_p_accepts_eventually() {
  ExplicitKripke k;
  auto p = k.add_atomic_proposition();

  auto s0 = k.add_state();
  auto s1 = k.add_state();

  k.add_initial(s0);
  k.add_edge(s0, s1);
  k.add_edge(s1, s1);

  k.set_holds(s0, p, false);
  k.set_holds(s1, p, true);

  auto f = Formula::make_finally(Formula::make_atom(p));
  auto ba = translate_to_buchi(f);

  auto prod = product_kripke_buchi(k, ba);
  auto res = buchi_emptiness(prod.automaton, true);

  assert(res.non_empty);
  assert(res.witness.has_value());
}

static void test_translate_F_p_rejects_if_never_true() {
  ExplicitKripke k;
  auto p = k.add_atomic_proposition();

  auto s0 = k.add_state();
  k.add_initial(s0);
  k.add_edge(s0, s0);
  k.set_holds(s0, p, false);

  auto f = Formula::make_finally(Formula::make_atom(p));
  auto ba = translate_to_buchi(f);

  auto prod = product_kripke_buchi(k, ba);
  auto res = buchi_emptiness(prod.automaton);

  assert(!res.non_empty);
}

int main() {
  test_translate_G_p_accepts_when_p_always_true();
  test_translate_G_p_rejects_when_p_false();
  test_translate_F_p_accepts_eventually();
  test_translate_F_p_rejects_if_never_true();
  return 0;
}
