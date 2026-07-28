
#include <omega/algorithms/scc.hpp>
#include <omega/automata/explicit_buchi.hpp>

#include <algorithm>
#include <cassert>
#include <vector>

using omega::algorithms::scc_tarjan;
using omega::automata::ExplicitBuchiAutomata;
using omega::core::StateId;

static void test_single_node_no_edges() {
  ExplicitBuchiAutomata a;
  auto q0 = a.add_state();

  auto res = scc_tarjan(a);
  assert(res.num_components == 1);
  assert(res.comp_of.size() == 1);
  assert(res.comp_of[q0.raw()] == 0);
  assert(res.components.size() == 1);
  assert(res.components[0].size() == 1);
  assert(res.components[0][0] == q0);
}

static void test_two_nodes_one_direction_edge() {
  ExplicitBuchiAutomata a;
  auto q0 = a.add_state();
  auto q1 = a.add_state();
  a.add_edge(q0, q1);

  auto res = scc_tarjan(a);
  assert(res.num_components == 2);
  assert(res.comp_of[q0.raw()] != res.comp_of[q1.raw()]);
}

static void test_two_nodes_cycle() {
  ExplicitBuchiAutomata a;
  auto q0 = a.add_state();
  auto q1 = a.add_state();
  a.add_edge(q0, q1);
  a.add_edge(q1, q0);

  auto res = scc_tarjan(a);
  assert(res.num_components == 1);
  assert(res.comp_of[q0.raw()] == res.comp_of[q1.raw()]);
  assert(res.components.size() == 1);
  assert(res.components[0].size() == 2);
}

static void test_multiple_sccs_with_self_loop() {
  ExplicitBuchiAutomata a;
  auto q0 = a.add_state();
  auto q1 = a.add_state();
  auto q2 = a.add_state();

  // SCC {q0, q1}
  a.add_edge(q0, q1);
  a.add_edge(q1, q0);

  // SCC {q2} with self-loop
  a.add_edge(q2, q2);

  auto res = scc_tarjan(a);
  assert(res.num_components == 2);

  const int c01 = res.comp_of[q0.raw()];
  const int c2 = res.comp_of[q2.raw()];
  assert(c01 == res.comp_of[q1.raw()]);
  assert(c01 != c2);

  // sanity: components contain correct sizes (order not guaranteed)
  std::vector<std::size_t> sizes;
  for (const auto &comp : res.components)
    sizes.push_back(comp.size());
  std::sort(sizes.begin(), sizes.end());
  assert((sizes == std::vector<std::size_t>{1, 2}));
}

int main() {
  test_single_node_no_edges();
  test_two_nodes_one_direction_edge();
  test_two_nodes_cycle();
  test_multiple_sccs_with_self_loop();
  return 0;
}
