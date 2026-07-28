#include <omega/automata/explicit_buchi.hpp>

#include <cassert>
#include <vector>

using omega::automata::ExplicitBuchiAutomata;
using omega::core::StateId;

static void test_empty_automaton_defaults() {
  ExplicitBuchiAutomata a;

  assert(a.num_states() == 0);
  assert(a.initial_states().size() == 0);
}

static void test_add_states_and_accepting_flags() {
  ExplicitBuchiAutomata a;

  auto q0 = a.add_state(false);
  auto q1 = a.add_state(true);

  assert(a.num_states() == 2);

  assert(q0.valid() && q1.valid());
  assert(q0.raw() == 0u);
  assert(q1.raw() == 1u);

  assert(a.is_accepting(q0) == false);
  assert(a.is_accepting(q1) == true);

  a.set_accepting(q0, true);
  assert(a.is_accepting(q0) == true);

  a.set_accepting(q1, false);
  assert(a.is_accepting(q1) == false);
}

static void test_initial_states() {
  ExplicitBuchiAutomata a;

  auto q0 = a.add_state();
  auto q1 = a.add_state();

  a.add_initial(q0);
  a.add_initial(q1);

  auto init = a.initial_states();
  assert(init.size() == 2);
  assert(init[0] == q0);
  assert(init[1] == q1);

  a.clear_initials();
  assert(a.initial_states().empty());
}

static void test_edges_and_out_edges_span() {
  ExplicitBuchiAutomata a;

  auto q0 = a.add_state();
  auto q1 = a.add_state();
  auto q2 = a.add_state();

  // initially no edges
  assert(a.out_edges(q0).size() == 0);

  a.add_edge(q0, q1);
  a.add_edge(q0, q2);
  a.add_edge(q2, q2); // self-loop

  auto e0 = a.out_edges(q0);
  assert(e0.size() == 2);
  assert(e0[0].dst == q1);
  assert(e0[1].dst == q2);

  auto e2 = a.out_edges(q2);
  assert(e2.size() == 1);
  assert(e2[0].dst == q2);
}

static void test_add_edge_ref_returns_reference_to_last_edge() {
  ExplicitBuchiAutomata a;

  auto q0 = a.add_state();
  auto q1 = a.add_state();

  auto &e = a.add_edge_ref(q0, q1);
  assert(e.dst == q1);

  // Ensure it really is the stored edge by mutating it (only dst exists right
  // now). This is mostly to confirm reference stability for the *current*
  // vector state. (Note: adding more edges later may reallocate and invalidate
  // references.)
  e.dst = q0;
  assert(a.out_edges(q0).size() == 1);
  assert(a.out_edges(q0)[0].dst == q0);
}

static void test_reserve_states_does_not_change_semantics() {
  ExplicitBuchiAutomata a;
  a.reserve_states(100);

  auto q0 = a.add_state();
  auto q1 = a.add_state(true);

  a.add_initial(q0);
  a.add_edge(q0, q1);

  assert(a.num_states() == 2);
  assert(a.initial_states().size() == 1);
  assert(a.initial_states()[0] == q0);
  assert(a.out_edges(q0).size() == 1);
  assert(a.out_edges(q0)[0].dst == q1);
  assert(a.is_accepting(q1) == true);
}

int main() {
  test_empty_automaton_defaults();
  test_add_states_and_accepting_flags();
  test_initial_states();
  test_edges_and_out_edges_span();
  test_add_edge_ref_returns_reference_to_last_edge();
  test_reserve_states_does_not_change_semantics();
  return 0;
}
