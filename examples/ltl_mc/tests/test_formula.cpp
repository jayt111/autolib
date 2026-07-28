#include "formula.hpp"

#include <cassert>

using ltl_mc::Formula;
using ltl_mc::FormulaKind;
using omega::core::APId;

static void test_constants_and_atom() {
  auto t = Formula::make_true();
  auto f = Formula::make_false();
  auto a = Formula::make_atom(APId{0});

  assert(t->kind() == FormulaKind::True);
  assert(f->kind() == FormulaKind::False);
  assert(a->kind() == FormulaKind::Atom);
  assert(a->atom() == APId{0});
}

static void test_unary_ops() {
  auto a = Formula::make_atom(APId{1});

  auto n = Formula::make_not(a);
  auto x = Formula::make_next(a);
  auto ff = Formula::make_finally(a);
  auto g = Formula::make_globally(a);

  assert(n->kind() == FormulaKind::Not);
  assert(x->kind() == FormulaKind::Next);
  assert(ff->kind() == FormulaKind::Finally);
  assert(g->kind() == FormulaKind::Globally);

  assert(n->left()->kind() == FormulaKind::Atom);
}

static void test_binary_ops() {
  auto a = Formula::make_atom(APId{0});
  auto b = Formula::make_atom(APId{1});

  auto conj = Formula::make_and(a, b);
  auto disj = Formula::make_or(a, b);
  auto until = Formula::make_until(a, b);
  auto release = Formula::make_release(a, b);

  assert(conj->kind() == FormulaKind::And);
  assert(disj->kind() == FormulaKind::Or);
  assert(until->kind() == FormulaKind::Until);
  assert(release->kind() == FormulaKind::Release);

  assert(conj->left()->kind() == FormulaKind::Atom);
  assert(conj->right()->kind() == FormulaKind::Atom);
}

int main() {
  test_constants_and_atom();
  test_unary_ops();
  test_binary_ops();
  return 0;
}
