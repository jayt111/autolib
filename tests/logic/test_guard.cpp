#include <omega/logic/guard.hpp>

#include <cassert>

using omega::core::APId;
using omega::logic::Guard;

static void test_true_guard() {
  Guard g;
  assert(g.empty());
  assert(g.is_consistent());
}

static void test_positive_and_negative_literals() {
  Guard g;
  APId a{0};
  APId b{1};

  g.require(a);
  g.forbid(b);

  assert(g.has_positive(a));
  assert(!g.has_negative(a));
  assert(g.has_negative(b));
  assert(!g.has_positive(b));
  assert(g.is_consistent());
}

static void test_inconsistent_guard() {
  Guard g;
  APId a{0};

  g.require(a);
  g.forbid(a);

  assert(!g.is_consistent());
}

int main() {
  test_true_guard();
  test_positive_and_negative_literals();
  test_inconsistent_guard();
  return 0;
}
