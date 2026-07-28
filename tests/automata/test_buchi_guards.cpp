#include <omega/automata/explicit_buchi.hpp>
#include <omega/logic/guard.hpp>

#include <cassert>

using omega::automata::ExplicitBuchiAutomata;
using omega::core::APId;
using omega::logic::Guard;

static void test_unguarded_edge_defaults_to_no_guard() {
  ExplicitBuchiAutomata a;

  auto q0 = a.add_state(false);
  auto q1 = a.add_state(true);

  a.add_edge(q0, q1);

  auto edges = a.out_edges(q0);
  assert(edges.size() == 1);
  assert(edges[0].dst == q1);
  assert(!edges[0].guard.has_value());
}

static void test_guarded_edge_stores_guard() {
  ExplicitBuchiAutomata a;

  auto q0 = a.add_state(false);
  auto q1 = a.add_state(true);

  APId ap0{0};
  APId ap1{1};

  Guard g;
  g.require(ap0);
  g.forbid(ap1);

  a.add_edge(q0, q1, g);

  auto edges = a.out_edges(q0);
  assert(edges.size() == 1);
  assert(edges[0].dst == q1);
  assert(edges[0].guard.has_value());

  const Guard &stored = *edges[0].guard;
  assert(stored.has_positive(ap0));
  assert(!stored.has_negative(ap0));
  assert(stored.has_negative(ap1));
  assert(!stored.has_positive(ap1));
  assert(stored.is_consistent());
}

int main() {
  test_unguarded_edge_defaults_to_no_guard();
  test_guarded_edge_stores_guard();
  return 0;
}
