#include <cassert>
#include <omega/acceptance/rabin.hpp>

using omega::acceptance::Rabin;
using omega::core::StateId;

static void test_resize_and_add_pair() {
  Rabin r;
  r.resize(3);
  assert(r.num_states() == 3);
  assert(r.num_pairs() == 0);

  auto p0 = r.add_pair();
  assert(p0 == 0);
  assert(r.num_pairs() == 1);

  // initially empty
  assert(!r.in_E(0, StateId{0}));
  assert(!r.in_F(0, StateId{2}));

  r.set_E(0, StateId{1}, true);
  r.set_F(0, StateId{2}, true);

  assert(r.in_E(0, StateId{1}));
  assert(r.in_F(0, StateId{2}));
}

static void test_resize_preserves_pairs_sizes() {
  Rabin r(2);
  r.add_pair();
  r.set_E(0, StateId{0}, true);

  r.resize(5);
  assert(r.num_states() == 5);
  assert(r.num_pairs() == 1);
  assert(r.in_E(0, StateId{0}));
  assert(!r.in_E(0, StateId{4}));
}

int main() {
  test_resize_and_add_pair();
  test_resize_preserves_pairs_sizes();
  return 0;
}
