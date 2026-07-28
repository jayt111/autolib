#include "formula.hpp"
#include "model_check.hpp"

#include <cassert>

using omega::core::APId;
using omega::kripke::ExplicitKripke;

using ltl_mc::Formula;
using ltl_mc::model_check;

static void test_model_check_G_p_holds() {
  ExplicitKripke k;
  auto p = k.add_atomic_proposition();

  auto s0 = k.add_state();
  k.add_initial(s0);
  k.add_edge(s0, s0);
  k.set_holds(s0, p, true);

  auto phi = Formula::make_globally(Formula::make_atom(p));
  auto res = model_check(k, phi);

  assert(res.holds);
  assert(!res.product_counterexample.has_value());
}

static void test_model_check_G_p_fails() {
  ExplicitKripke k;
  auto p = k.add_atomic_proposition();

  auto s0 = k.add_state();
  k.add_initial(s0);
  k.add_edge(s0, s0);
  k.set_holds(s0, p, false);

  auto phi = Formula::make_globally(Formula::make_atom(p));
  auto res = model_check(k, phi);

  assert(!res.holds);
  assert(res.product_counterexample.has_value());
}

static void test_model_check_F_p_holds() {
  ExplicitKripke k;
  auto p = k.add_atomic_proposition();

  auto s0 = k.add_state();
  auto s1 = k.add_state();

  k.add_initial(s0);
  k.add_edge(s0, s1);
  k.add_edge(s1, s1);

  k.set_holds(s0, p, false);
  k.set_holds(s1, p, true);

  auto phi = Formula::make_finally(Formula::make_atom(p));
  auto res = model_check(k, phi);

  assert(res.holds);
  assert(!res.product_counterexample.has_value());
}

static void test_model_check_F_p_fails() {
  ExplicitKripke k;
  auto p = k.add_atomic_proposition();

  auto s0 = k.add_state();
  k.add_initial(s0);
  k.add_edge(s0, s0);
  k.set_holds(s0, p, false);

  auto phi = Formula::make_finally(Formula::make_atom(p));
  auto res = model_check(k, phi);

  assert(!res.holds);
  assert(res.product_counterexample.has_value());
}

static void test_model_check_G_p_fails_with_kripke_trace() {
  ExplicitKripke k;
  auto p = k.add_atomic_proposition();

  auto s0 = k.add_state();
  k.add_initial(s0);
  k.add_edge(s0, s0);
  k.set_holds(s0, p, false);

  auto phi = Formula::make_globally(Formula::make_atom(p));
  auto res = model_check(k, phi);

  assert(!res.holds);
  assert(res.kripke_counterexample.has_value());

  const auto &w = *res.kripke_counterexample;
  assert(!w.prefix.empty());
  assert(!w.cycle.empty());

  // Since the only Kripke state is s0, everything should project to s0
  for (auto s : w.prefix) {
    assert(s == s0);
  }
  for (auto s : w.cycle) {
    assert(s == s0);
  }
}
int main() {
  test_model_check_G_p_holds();
  test_model_check_G_p_fails();
  test_model_check_F_p_holds();
  test_model_check_F_p_fails();
  return 0;
}
