#pragma once

#include <stdexcept>

#include "formula.hpp"

namespace ltl_mc {

namespace detail {

inline FormulaPtr nnf(const FormulaPtr &f, bool neg) {
  using K = FormulaKind;

  switch (f->kind()) {
  case K::True:
    return neg ? Formula::make_false() : Formula::make_true();

  case K::False:
    return neg ? Formula::make_true() : Formula::make_false();

  case K::Atom:
    return neg ? Formula::make_not(Formula::make_atom(f->atom()))
               : Formula::make_atom(f->atom());

  case K::Not:
    // Push negation inward by flipping the flag
    return nnf(f->left(), !neg);

  case K::And:
    if (!neg) {
      return Formula::make_and(nnf(f->left(), false), nnf(f->right(), false));
    } else {
      return Formula::make_or(nnf(f->left(), true), nnf(f->right(), true));
    }

  case K::Or:
    if (!neg) {
      return Formula::make_or(nnf(f->left(), false), nnf(f->right(), false));
    } else {
      return Formula::make_and(nnf(f->left(), true), nnf(f->right(), true));
    }

  case K::Next:
    // !X p == X !p
    return Formula::make_next(nnf(f->left(), neg));

  case K::Finally:
    // !(F p) == G !p
    if (!neg) {
      return Formula::make_finally(nnf(f->left(), false));
    } else {
      return Formula::make_globally(nnf(f->left(), true));
    }

  case K::Globally:
    // !(G p) == F !p
    if (!neg) {
      return Formula::make_globally(nnf(f->left(), false));
    } else {
      return Formula::make_finally(nnf(f->left(), true));
    }

  case K::Until:
    // !(p U p1) == (!p) R (!p1)
    if (!neg) {
      return Formula::make_until(nnf(f->left(), false), nnf(f->right(), false));
    } else {
      return Formula::make_release(nnf(f->left(), true), nnf(f->right(), true));
    }

  case K::Release:
    // !(p R p1) == (!p) U (!p1)
    if (!neg) {
      return Formula::make_release(nnf(f->left(), false),
                                   nnf(f->right(), false));
    } else {
      return Formula::make_until(nnf(f->left(), true), nnf(f->right(), true));
    }
  }

  throw std::runtime_error("to_nnf(): unknown formula kind");
}

} // namespace detail

inline FormulaPtr to_nnf(const FormulaPtr &f) { return detail::nnf(f, false); }

inline FormulaPtr negate(const FormulaPtr &f) { return detail::nnf(f, true); }

} // namespace ltl_mc
