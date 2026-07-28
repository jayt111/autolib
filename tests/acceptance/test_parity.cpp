#include <cassert>
#include <omega/acceptance/parity.hpp>

using omega::acceptance::Parity;
using omega::core::StateId;

static void test_resize_defaults() {
  Parity p;
  p.resize(3);
  assert(p.size() == 3);
  assert(p.priority(StateId{0}) == 0);
  assert(p.priority(StateId{1}) == 0);
  assert(p.priority(StateId{2}) == 0);
  assert(p.max_priority() == 0);
}

static void test_set_priority_and_max() {
  Parity p;
  p.resize(4);

  p.set_priority(StateId{0}, 2);
  p.set_priority(StateId{1}, 7);
  p.set_priority(StateId{2}, 1);

  assert(p.priority(StateId{0}) == 2);
  assert(p.priority(StateId{1}) == 7);
  assert(p.priority(StateId{2}) == 1);
  assert(p.max_priority() == 7);
}

int main() {
  test_resize_defaults();
  test_set_priority_and_max();
  return 0;
}
