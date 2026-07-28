#include <omega/algorithms/emptiness_parity.hpp>
#include <omega/automata/explicit_parity.hpp>

#include <cassert>

using omega::algorithms::parity_emptiness;
using omega::automata::ExplicitParityAutomaton;
using omega::core::StateId;

static void test_single_state_even_selfloop_nonempty() {
  ExplicitParityAutomaton a;
  auto q0 = a.add_state();
  a.add_initial(q0);
  a.add_edge(q0, q0);
  a.set_priority(q0, 2);

  auto r = parity_emptiness(a, true);
  assert(r.non_empty);
  assert(r.witness.has_value());
  assert(r.witness->cycle.size() >= 2);
  assert(r.witness->cycle.front() == r.witness->cycle.back());
  assert(r.witness->min_priority == 2);
}

static void test_single_state_odd_selfloop_empty() {
  ExplicitParityAutomaton a;
  auto q0 = a.add_state();
  a.add_initial(q0);
  a.add_edge(q0, q0);
  a.set_priority(q0, 3);

  auto r = parity_emptiness(a, true);
  assert(!r.non_empty);
  assert(!r.witness.has_value());
}

static void test_two_state_cycle_min_even_nonempty() {
  ExplicitParityAutomaton a;
  auto q0 = a.add_state();
  auto q1 = a.add_state();
  a.add_initial(q0);

  a.add_edge(q0, q1);
  a.add_edge(q1, q0);

  a.set_priority(q0, 5);
  a.set_priority(q1, 2); // min priority in SCC is 2 (even)

  auto r = parity_emptiness(a, true);
  assert(r.non_empty);
  assert(r.witness.has_value());
  assert(r.witness->min_priority == 2);
  assert(!r.witness->cycle.empty());
  assert(r.witness->cycle.front() == r.witness->cycle.back());
}

static void test_two_state_cycle_min_odd_empty() {
  ExplicitParityAutomaton a;
  auto q0 = a.add_state();
  auto q1 = a.add_state();
  a.add_initial(q0);

  a.add_edge(q0, q1);
  a.add_edge(q1, q0);

  a.set_priority(q0, 1);
  a.set_priority(q1, 4); // min priority in SCC is 1 (odd)

  auto r = parity_emptiness(a, true);
  assert(!r.non_empty);
  assert(!r.witness.has_value());
}

static void test_unreachable_good_scc_ignored() {
  ExplicitParityAutomaton a;

  // reachable trivial part (no cycle)
  auto init = a.add_state();
  a.add_initial(init);
  a.set_priority(init, 7);

  // unreachable even self-loop SCC
  auto dead = a.add_state();
  a.set_priority(dead, 0);
  a.add_edge(dead, dead);

  auto r = parity_emptiness(a, true);
  assert(!r.non_empty);
  assert(!r.witness.has_value());
}

static void test_reachable_acyclic_even_state_is_empty() {
  ExplicitParityAutomaton a;
  auto q0 = a.add_state();
  auto q1 = a.add_state();
  a.add_initial(q0);

  a.add_edge(q0, q1); // no cycle anywhere
  a.set_priority(q0, 2);
  a.set_priority(q1, 0);

  auto r = parity_emptiness(a, true);
  assert(!r.non_empty);
  assert(!r.witness.has_value());
}

static void test_multiple_sccs_only_one_good() {
  ExplicitParityAutomaton a;

  // SCC A: reachable odd cycle (bad)
  auto q0 = a.add_state();
  auto q1 = a.add_state();
  a.add_initial(q0);
  a.add_edge(q0, q1);
  a.add_edge(q1, q0);
  a.set_priority(q0, 3);
  a.set_priority(q1, 6); // min=3 odd => bad SCC

  // Path to SCC B
  auto q2 = a.add_state();
  a.add_edge(q1, q2);

  // SCC B: reachable even self-loop (good)
  auto q3 = a.add_state();
  a.add_edge(q2, q3);
  a.add_edge(q3, q3);
  a.set_priority(q2, 9);
  a.set_priority(q3, 0); // min=0 even => good SCC

  auto r = parity_emptiness(a, true);
  assert(r.non_empty);
  assert(r.witness.has_value());
  assert(r.witness->min_priority == 0);
  assert(!r.witness->cycle.empty());
  assert(r.witness->cycle.front() == r.witness->cycle.back());
}

int main() {
  test_single_state_even_selfloop_nonempty();
  test_single_state_odd_selfloop_empty();
  test_two_state_cycle_min_even_nonempty();
  test_two_state_cycle_min_odd_empty();
  test_unreachable_good_scc_ignored();
  test_reachable_acyclic_even_state_is_empty();
  test_multiple_sccs_only_one_good();
  return 0;
}
