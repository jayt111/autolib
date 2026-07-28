
#include <omega/algorithms/emptiness_buchi.hpp>
#include <omega/automata/explicit_buchi.hpp>

#include <cassert>

using omega::algorithms::buchi_emptiness;
using omega::automata::ExplicitBuchiAutomata;

static void test_empty_language_no_states() {
  ExplicitBuchiAutomata a;
  auto r = buchi_emptiness(a);
  assert(r.non_empty == false);
}

static void test_empty_language_no_initials() {
  ExplicitBuchiAutomata a;
  (void)a.add_state(true);
  auto r = buchi_emptiness(a);
  assert(r.non_empty == false);
}

static void test_accepting_but_no_cycle_is_empty() {
  ExplicitBuchiAutomata a;
  auto q0 = a.add_state(false);
  auto q1 = a.add_state(true);

  a.add_initial(q0);
  a.add_edge(q0, q1);
  // no edge back, no self-loop on q1 => no accepting cycle

  auto r = buchi_emptiness(a);
  assert(r.non_empty == false);
}

static void test_accepting_self_loop_is_non_empty() {
  ExplicitBuchiAutomata a;
  auto q0 = a.add_state(false);
  auto q1 = a.add_state(true);

  a.add_initial(q0);
  a.add_edge(q0, q1);
  a.add_edge(q1, q1); // accepting cycle

  auto r = buchi_emptiness(a);
  assert(r.non_empty == true);
  assert(r.witness_scc >= 0);
}

static void test_accepting_two_state_cycle_is_non_empty() {
  ExplicitBuchiAutomata a;
  auto q0 = a.add_state(false);
  auto q1 = a.add_state(true);
  auto q2 = a.add_state(false);

  a.add_initial(q0);
  a.add_edge(q0, q1);
  a.add_edge(q1, q2);
  a.add_edge(q2, q1); // cycle SCC {q1,q2} contains accepting q1

  auto r = buchi_emptiness(a);
  assert(r.non_empty == true);
  assert(r.witness_scc >= 0);
}

static void test_unreachable_accepting_cycle_is_empty() {
  ExplicitBuchiAutomata a;

  auto init = a.add_state(false);
  a.add_initial(init);

  auto dead = a.add_state(true);
  a.add_edge(dead, dead); // accepting cycle but unreachable

  auto r = buchi_emptiness(a);
  assert(r.non_empty == false);
}

int main() {
  test_empty_language_no_states();
  test_empty_language_no_initials();
  test_accepting_but_no_cycle_is_empty();
  test_accepting_self_loop_is_non_empty();
  test_accepting_two_state_cycle_is_non_empty();
  test_unreachable_accepting_cycle_is_empty();
  return 0;
}
