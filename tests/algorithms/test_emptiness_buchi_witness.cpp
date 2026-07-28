
#include <omega/algorithms/emptiness_buchi.hpp>
#include <omega/automata/explicit_buchi.hpp>

#include <cassert>

using omega::algorithms::buchi_emptiness;
using omega::automata::ExplicitBuchiAutomata;
using omega::core::StateId;

static bool has_edge(const ExplicitBuchiAutomata &a, StateId u, StateId v) {
  for (const auto &e : a.out_edges(u))
    if (e.dst == v)
      return true;
  return false;
}

static void assert_path_valid(const ExplicitBuchiAutomata &a,
                              const std::vector<StateId> &path) {
  assert(!path.empty());
  for (std::size_t i = 0; i + 1 < path.size(); ++i) {
    assert(has_edge(a, path[i], path[i + 1]));
  }
}

static void test_self_loop_witness() {
  ExplicitBuchiAutomata a;
  auto q0 = a.add_state(false);
  auto q1 = a.add_state(true);

  a.add_initial(q0);
  a.add_edge(q0, q1);
  a.add_edge(q1, q1);

  auto r = buchi_emptiness(a, true);
  assert(r.non_empty);
  assert(r.witness.has_value());

  const auto &w = *r.witness;
  assert(!w.prefix.empty());
  assert(w.prefix.front() == q0);
  assert(w.prefix.back() == q1);
  assert_path_valid(a, w.prefix);

  assert(w.cycle.size() >= 2);
  assert(w.cycle.front() == w.cycle.back());
  assert(w.cycle.front() == q1);
  assert_path_valid(a, w.cycle);

  // accepting appears on the cycle (we pick accepting as cycle start)
  assert(a.is_accepting(w.cycle.front()));
}

static void test_two_state_cycle_witness() {
  ExplicitBuchiAutomata a;
  auto q0 = a.add_state(false);
  auto q1 = a.add_state(true);
  auto q2 = a.add_state(false);

  a.add_initial(q0);
  a.add_edge(q0, q1);
  a.add_edge(q1, q2);
  a.add_edge(q2, q1);

  auto r = buchi_emptiness(a, true);
  assert(r.non_empty);
  assert(r.witness.has_value());

  const auto &w = *r.witness;
  assert(!w.prefix.empty());
  assert(w.prefix.front() == q0);
  assert(w.prefix.back() == q1);
  assert_path_valid(a, w.prefix);

  assert(w.cycle.front() == w.cycle.back());
  assert(w.cycle.front() == q1);
  assert_path_valid(a, w.cycle);
  assert(a.is_accepting(w.cycle.front()));
}

static void test_unreachable_accepting_cycle_no_witness() {
  ExplicitBuchiAutomata a;
  auto init = a.add_state(false);
  a.add_initial(init);

  auto dead = a.add_state(true);
  a.add_edge(dead, dead); // accepting cycle but unreachable

  auto r = buchi_emptiness(a, true);
  assert(!r.non_empty);
  assert(!r.witness.has_value());
}

int main() {
  test_self_loop_witness();
  test_two_state_cycle_witness();
  test_unreachable_accepting_cycle_no_witness();
  return 0;
}
