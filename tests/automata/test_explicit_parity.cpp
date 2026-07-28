#include <omega/automata/explicit_parity.hpp>
#include <omega/core/concepts.hpp>

#include <cassert>

using omega::automata::ExplicitParityAutomaton;
using omega::core::StateId;

static_assert(omega::core::ParityAutomaton<ExplicitParityAutomaton>);

static void test_build_and_query() {
  ExplicitParityAutomaton a;

  auto q0 = a.add_state();
  auto q1 = a.add_state();

  a.add_initial(q0);
  a.add_edge(q0, q1);

  // default priorities
  assert(a.priority(q0) == 0);
  assert(a.priority(q1) == 0);

  a.set_priority(q0, 3);
  a.set_priority(q1, 2);

  assert(a.priority(q0) == 3);
  assert(a.priority(q1) == 2);
  assert(a.num_states() == 2);
  assert(a.out_edges(q0).size() == 1);
  assert(a.out_edges(q0)[0].dst == q1);
}

int main() {
  test_build_and_query();
  return 0;
}
