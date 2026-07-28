#include "formula.hpp"
#include "negate.hpp"

#include <cassert>

using ltl_mc::Formula;
using ltl_mc::FormulaKind;
using ltl_mc::FormulaPtr;
using ltl_mc::negate;
using ltl_mc::to_nnf;
using omega::core::APId;

static void test_double_negation_atom() {
  auto p = Formula::make_atom(APId{0});
  auto f = Formula::make_not(Formula::make_not(p));

  auto nnf = to_nnf(f);

  assert(nnf->kind() == FormulaKind::Atom);
  assert(nnf->atom() == APId{0});
}

static void test_de_morgan_and() {
  auto p = Formula::make_atom(APId{0});
  auto q = Formula::make_atom(APId{1});
  auto f = Formula::make_not(Formula::make_and(p, q));

  auto nnf = to_nnf(f);

  assert(nnf->kind() == FormulaKind::Or);

  assert(nnf->left()->kind() == FormulaKind::Not);
  assert(nnf->left()->left()->kind() == FormulaKind::Atom);
  assert(nnf->left()->left()->atom() == APId{0});

  assert(nnf->right()->kind() == FormulaKind::Not);
  assert(nnf->right()->left()->kind() == FormulaKind::Atom);
  assert(nnf->right()->left()->atom() == APId{1});
}

static void test_de_morgan_or() {
  auto p = Formula::make_atom(APId{0});
  auto q = Formula::make_atom(APId{1});
  auto f = Formula::make_not(Formula::make_or(p, q));

  auto nnf = to_nnf(f);

  assert(nnf->kind() == FormulaKind::And);

  assert(nnf->left()->kind() == FormulaKind::Not);
  assert(nnf->right()->kind() == FormulaKind::Not);
}

static void test_negate_finally() {
  auto p = Formula::make_atom(APId{0});
  auto f = Formula::make_finally(p);

  auto g = negate(f);

  assert(g->kind() == FormulaKind::Globally);
  assert(g->left()->kind() == FormulaKind::Not);
  assert(g->left()->left()->kind() == FormulaKind::Atom);
}

static void test_negate_globally() {
  auto p = Formula::make_atom(APId{0});
  auto f = Formula::make_globally(p);

  auto g = negate(f);

  assert(g->kind() == FormulaKind::Finally);
  assert(g->left()->kind() == FormulaKind::Not);
  assert(g->left()->left()->kind() == FormulaKind::Atom);
}

static void test_negate_until() {
  auto p = Formula::make_atom(APId{0});
  auto q = Formula::make_atom(APId{1});
  auto f = Formula::make_until(p, q);

  auto g = negate(f);

  assert(g->kind() == FormulaKind::Release);
  assert(g->left()->kind() == FormulaKind::Not);
  assert(g->right()->kind() == FormulaKind::Not);
}

static void test_negate_release() {
  auto p = Formula::make_atom(APId{0});
  auto q = Formula::make_atom(APId{1});
  auto f = Formula::make_release(p, q);

  auto g = negate(f);

  assert(g->kind() == FormulaKind::Until);
  assert(g->left()->kind() == FormulaKind::Not);
  assert(g->right()->kind() == FormulaKind::Not);
}

static void test_or_preserves_both_sides() {
  auto p = Formula::make_atom(APId{0});
  auto q = Formula::make_atom(APId{1});
  auto f = Formula::make_or(p, q);

  auto nnf = to_nnf(f);

  assert(nnf->kind() == FormulaKind::Or);
  assert(nnf->left()->kind() == FormulaKind::Atom);
  assert(nnf->left()->atom() == APId{0});
  assert(nnf->right()->kind() == FormulaKind::Atom);
  assert(nnf->right()->atom() == APId{1});
}

int main() {
  test_double_negation_atom();
  test_de_morgan_and();
  test_de_morgan_or();
  test_negate_finally();
  test_negate_globally();
  test_negate_until();
  test_negate_release();
  test_or_preserves_both_sides();
  return 0;
}
