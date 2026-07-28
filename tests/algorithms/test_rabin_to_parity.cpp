#include <omega/algorithms/emptiness_parity.hpp>
#include <omega/algorithms/rabin_to_parity.hpp>
#include <omega/automata/explicit_parity.hpp>
#include <omega/automata/explicit_rabin.hpp>

#include <cassert>
#include <cstddef>

using omega::algorithms::parity_emptiness;
using omega::algorithms::rabin_to_parity;
using omega::automata::ExplicitParityAutomaton;
using omega::automata::ExplicitRabinAutomaton;
using omega::core::Priority;
using omega::core::StateId;

static StateId sid(std::size_t i) {
  return StateId{static_cast<StateId::underlying_type>(i)};
}

static void test_shape_and_priorities_single_pair() {
  ExplicitRabinAutomaton a;

  auto q0 = a.add_state();
  auto q1 = a.add_state();
  a.add_initial(q0);
  a.add_edge(q0, q1);
  a.add_edge(q1, q1);

  const std::size_t k = 1;
  const std::size_t n = a.num_states();

  auto pidx = a.add_pair(); // 0

  // Put q0 in E, q1 in F
  a.set_E(pidx, q0, true);
  a.set_F(pidx, q1, true);

  ExplicitParityAutomaton p = rabin_to_parity(a);

  // states = n * k
  assert(p.num_states() == n * k);

  // initials = |I| * k
  assert(p.initial_states().size() == a.initial_states().size() * k);

  // Since k=1, state indices correspond to original q indices:
  // (pair 0, q0) -> 0, (pair 0, q1) -> 1
  assert(p.priority(sid(0)) == Priority{1}); // E => 1
  assert(p.priority(sid(1)) == Priority{2}); // F => 2

  auto r = parity_emptiness(p, true);
  assert(r.non_empty);
  assert(r.witness.has_value());
}

static void test_two_pairs_priorities_and_initial_multiplicity() {
  ExplicitRabinAutomaton a;

  auto q0 = a.add_state();
  auto q1 = a.add_state();
  a.add_initial(q0);
  a.add_edge(q0, q1);
  a.add_edge(q1, q1);

  const std::size_t n = a.num_states();
  assert(n == 2);

  auto i0 = a.add_pair(); // 0
  auto i1 = a.add_pair(); // 1
  assert(a.num_pairs() == 2);

  // Pair 0: E={q0}, F=empty
  a.set_E(i0, q0, true);

  // Pair 1: E={q0}, F={q1}
  a.set_E(i1, q0, true);
  a.set_F(i1, q1, true);

  ExplicitParityAutomaton p = rabin_to_parity(a);

  const std::size_t k = 2;
  assert(p.num_states() == n * k);
  assert(p.initial_states().size() == a.initial_states().size() * k);

  // Expected ordering in the construction loops is:
  // pair 0: q0,q1 -> ids 0,1
  // pair 1: q0,q1 -> ids 2,3
  //
  // Check priorities:
  // pair 0:
  assert(p.priority(sid(0)) == Priority{1}); // q0 in E0
  assert(p.priority(sid(1)) ==
         Priority{3}); // q1 in neither => 3 (since F0 empty)
  // pair 1:
  assert(p.priority(sid(2)) == Priority{1}); // q0 in E1
  assert(p.priority(sid(3)) == Priority{2}); // q1 in F1

  auto r = parity_emptiness(p, true);
  assert(r.non_empty);
  assert(r.witness.has_value());
  assert(r.witness->min_priority == Priority{2} ||
         r.witness->min_priority == Priority{0});
}

static void test_no_pairs_is_empty() {
  ExplicitRabinAutomaton a;

  auto q0 = a.add_state();
  a.add_initial(q0);
  a.add_edge(q0, q0);

  // no pairs
  assert(a.num_pairs() == 0);

  auto p = rabin_to_parity(a);
  auto r = parity_emptiness(p);

  assert(!r.non_empty);
}

int main() {
  test_shape_and_priorities_single_pair();
  test_two_pairs_priorities_and_initial_multiplicity();
  test_no_pairs_is_empty();
  return 0;
}
