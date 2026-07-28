#include <omega/algorithms/emptiness_buchi.hpp>
#include <omega/algorithms/product.hpp>
#include <omega/automata/explicit_buchi.hpp>

#include <cassert>

using omega::algorithms::buchi_emptiness;
using omega::algorithms::product_buchi;
using omega::automata::ExplicitBuchiAutomata;

static void test_product_shape() {
  ExplicitBuchiAutomata a;
  auto a0 = a.add_state(false);
  auto a1 = a.add_state(true);
  a.add_initial(a0);
  a.add_edge(a0, a1);
  a.add_edge(a1, a1);

  ExplicitBuchiAutomata b;
  auto b0 = b.add_state(false);
  auto b1 = b.add_state(true);
  auto b2 = b.add_state(false);
  b.add_initial(b0);
  b.add_edge(b0, b1);
  b.add_edge(b1, b2);
  b.add_edge(b2, b1);

  auto p = product_buchi(a, b);

  // 2 phases * |A| * |B|
  assert(p.num_states() == 2 * a.num_states() * b.num_states());

  // |IA| * |IB|
  assert(p.initial_states().size() ==
         a.initial_states().size() * b.initial_states().size());
}

static void test_product_nonempty_when_both_nonempty() {
  ExplicitBuchiAutomata a;
  auto a0 = a.add_state(false);
  auto a1 = a.add_state(true);
  a.add_initial(a0);
  a.add_edge(a0, a1);
  a.add_edge(a1, a1); // accepting loop

  ExplicitBuchiAutomata b;
  auto b0 = b.add_state(false);
  auto b1 = b.add_state(true);
  b.add_initial(b0);
  b.add_edge(b0, b1);
  b.add_edge(b1, b1); // accepting loop

  auto p = product_buchi(a, b);
  auto r = buchi_emptiness(p, true);

  assert(r.non_empty);
  assert(r.witness.has_value());
}

static void test_product_empty_when_one_empty() {
  ExplicitBuchiAutomata a;
  auto a0 = a.add_state(false);
  auto a1 = a.add_state(true);
  a.add_initial(a0);
  a.add_edge(a0, a1);
  a.add_edge(a1, a1); // non-empty

  ExplicitBuchiAutomata b;
  auto b0 = b.add_state(false);
  auto b1 = b.add_state(true);
  b.add_initial(b0);
  b.add_edge(b0, b1);
  // no self-loop / no cycle through accepting state => empty

  auto p = product_buchi(a, b);
  auto r = buchi_emptiness(p);

  assert(!r.non_empty);
}

static void test_product_needs_phase_bit() {
  // A accepts by visiting a1 infinitely often
  ExplicitBuchiAutomata a;
  auto a0 = a.add_state(false);
  auto a1 = a.add_state(true);
  a.add_initial(a0);
  a.add_edge(a0, a1);
  a.add_edge(a1, a1);

  // B accepts by visiting b1 infinitely often, but "both accepting at once"
  // is not the right criterion in general. This test just exercises phase
  // switching.
  ExplicitBuchiAutomata b;
  auto b0 = b.add_state(false);
  auto b1 = b.add_state(true);
  b.add_initial(b0);
  b.add_edge(b0, b1);
  b.add_edge(b1, b1);

  auto p = product_buchi(a, b);
  auto r = buchi_emptiness(p, true);

  assert(r.non_empty);
  assert(r.witness.has_value());
}

int main() {
  test_product_shape();
  test_product_nonempty_when_both_nonempty();
  test_product_empty_when_one_empty();
  test_product_needs_phase_bit();
  return 0;
}
