#include <omega/automata/explicit_rabin.hpp>
#include <omega/core/concepts.hpp>

#include <cassert>

using omega::automata::ExplicitRabinAutomaton;
using omega::core::StateId;

static_assert(omega::core::RabinAutomaton<ExplicitRabinAutomaton>);

static void test_build_and_membership() {
  ExplicitRabinAutomaton a;

  auto q0 = a.add_state();
  auto q1 = a.add_state();
  a.add_initial(q0);
  a.add_edge(q0, q1);

  auto pair = a.add_pair();
  assert(pair == 0);
  assert(a.num_pairs() == 1);

  a.set_E(0, q0, true);
  a.set_F(0, q1, true);

  assert(a.in_E(0, q0));
  assert(!a.in_E(0, q1));
  assert(a.in_F(0, q1));
}

int main() {
  test_build_and_membership();
  return 0;
}
