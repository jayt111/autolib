#include <omega/algorithms/emptiness_buchi.hpp>
#include <omega/automata/explicit_buchi.hpp>
#include <omega/core/types.hpp>
#include <omega/kripke/explicit_kripke.hpp>
#include <omega/kripke/product_kripke_buchi.hpp>
#include <omega/logic/guard.hpp>

#include <cassert>

using omega::algorithms::buchi_emptiness;
using omega::automata::ExplicitBuchiAutomata;
using omega::core::APId;
using omega::kripke::ExplicitKripke;
using omega::kripke::product_kripke_buchi;
using omega::logic::Guard;

static void test_product_shape() {
  ExplicitKripke k;
  auto s0 = k.add_state();
  auto s1 = k.add_state();
  k.add_initial(s0);
  k.add_edge(s0, s1);
  k.add_edge(s1, s1);

  ExplicitBuchiAutomata a;
  auto q0 = a.add_state(false);
  auto q1 = a.add_state(true);
  a.add_initial(q0);
  a.add_edge(q0, q1);
  a.add_edge(q1, q1);

  auto res = product_kripke_buchi(k, a);
  auto p = res.automaton;

  assert(p.num_states() == k.num_states() * a.num_states());
  assert(p.initial_states().size() ==
         k.initial_states().size() * a.initial_states().size());
}

static void test_unguarded_product_nonempty() {
  ExplicitKripke k;
  auto s0 = k.add_state();
  auto s1 = k.add_state();
  k.add_initial(s0);
  k.add_edge(s0, s1);
  k.add_edge(s1, s1);

  ExplicitBuchiAutomata a;
  auto q0 = a.add_state(false);
  auto q1 = a.add_state(true);
  a.add_initial(q0);
  a.add_edge(q0, q1);
  a.add_edge(q1, q1);

  auto res = product_kripke_buchi(k, a);
  auto p = res.automaton;
  auto r = buchi_emptiness(p, true);

  assert(r.non_empty);
  assert(r.witness.has_value());
}

static void test_guard_blocks_transition() {
  ExplicitKripke k;
  auto ap = k.add_atomic_proposition();

  auto s0 = k.add_state();
  auto s1 = k.add_state();
  k.add_initial(s0);
  k.add_edge(s0, s1);
  k.add_edge(s1, s1);

  // AP does NOT hold at s1, so guarded edge should be disabled
  k.set_holds(s0, ap, false);
  k.set_holds(s1, ap, true);

  ExplicitBuchiAutomata a;
  auto q0 = a.add_state(false);
  auto q1 = a.add_state(true);
  a.add_initial(q0);

  Guard g;
  g.require(ap);

  a.add_edge(q0, q1, g);
  a.add_edge(q1, q1);

  auto res = product_kripke_buchi(k, a);
  auto p = res.automaton;
  auto r = buchi_emptiness(p);

  assert(!r.non_empty);
}

static void test_guard_enables_transition() {
  ExplicitKripke k;
  auto ap = k.add_atomic_proposition();

  auto s0 = k.add_state();
  auto s1 = k.add_state();
  k.add_initial(s0);
  k.add_edge(s0, s1);
  k.add_edge(s1, s1);

  // AP holds at s1, so guarded edge should be enabled
  k.set_holds(s0, ap, true);
  k.set_holds(s1, ap, false);

  ExplicitBuchiAutomata a;
  auto q0 = a.add_state(false);
  auto q1 = a.add_state(true);
  a.add_initial(q0);

  Guard g;
  g.require(ap);

  a.add_edge(q0, q1, g);
  a.add_edge(q1, q1);

  auto res = product_kripke_buchi(k, a);
  auto p = res.automaton;
  auto r = buchi_emptiness(p, true);

  assert(r.non_empty);
  assert(r.witness.has_value());
}

int main() {
  test_product_shape();
  test_unguarded_product_nonempty();
  test_guard_blocks_transition();
  test_guard_enables_transition();
  return 0;
}
