#include <omega/kripke/explicit_kripke.hpp>

#include <cassert>

using omega::core::APId;
using omega::core::StateId;
using omega::kripke::ExplicitKripke;

static void test_empty_defaults() {
  ExplicitKripke k;

  assert(k.num_states() == 0);
  assert(k.num_atomic_propositions() == 0);
  assert(k.initial_states().empty());
}

static void test_add_aps_before_states() {
  ExplicitKripke k;

  auto p0 = k.add_atomic_proposition();
  auto p1 = k.add_atomic_proposition();

  assert(p0.raw() == 0u);
  assert(p1.raw() == 1u);
  assert(k.num_atomic_propositions() == 2);

  auto s0 = k.add_state();
  assert(k.num_states() == 1);
  assert(!k.holds(s0, p0));
  assert(!k.holds(s0, p1));
}

static void test_add_states_before_aps() {
  ExplicitKripke k;

  auto s0 = k.add_state();
  auto s1 = k.add_state();

  auto p0 = k.add_atomic_proposition();
  auto p1 = k.add_atomic_proposition();

  assert(k.num_states() == 2);
  assert(k.num_atomic_propositions() == 2);

  assert(!k.holds(s0, p0));
  assert(!k.holds(s0, p1));
  assert(!k.holds(s1, p0));
  assert(!k.holds(s1, p1));
}

static void test_labeling() {
  ExplicitKripke k;

  auto p0 = k.add_atomic_proposition();
  auto p1 = k.add_atomic_proposition();

  auto s0 = k.add_state();
  auto s1 = k.add_state();

  k.set_holds(s0, p0, true);
  k.set_holds(s1, p1, true);

  assert(k.holds(s0, p0));
  assert(!k.holds(s0, p1));
  assert(!k.holds(s1, p0));
  assert(k.holds(s1, p1));

  k.clear_labeling(s0);
  assert(!k.holds(s0, p0));
  assert(!k.holds(s0, p1));
}

static void test_initials_and_edges() {
  ExplicitKripke k;

  auto s0 = k.add_state();
  auto s1 = k.add_state();
  auto s2 = k.add_state();

  k.add_initial(s0);
  k.add_initial(s2);

  auto init = k.initial_states();
  assert(init.size() == 2);
  assert(init[0] == s0);
  assert(init[1] == s2);

  k.add_edge(s0, s1);
  k.add_edge(s0, s2);
  k.add_edge(s2, s2);

  auto e0 = k.out_edges(s0);
  assert(e0.size() == 2);
  assert(e0[0].dst == s1);
  assert(e0[1].dst == s2);

  auto e2 = k.out_edges(s2);
  assert(e2.size() == 1);
  assert(e2[0].dst == s2);
}

int main() {
  test_empty_defaults();
  test_add_aps_before_states();
  test_add_states_before_aps();
  test_labeling();
  test_initials_and_edges();
  return 0;
}
