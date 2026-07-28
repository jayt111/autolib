#pragma once

#include <stdexcept>

#include "formula.hpp"

#include <omega/automata/explicit_buchi.hpp>
#include <omega/logic/guard.hpp>

// NOTE: currently supports p, !p, F p, F !p, G p, G !p
// TODO: Add support for rest

namespace ltl_mc {

namespace detail {

inline omega::logic::Guard guard_for_atom(omega::core::APId ap, bool negated) {
  omega::logic::Guard g;
  if (negated) {
    g.forbid(ap);
  } else {
    g.require(ap);
  }
  return g;
}

// G p  or  G !p
inline omega::automata::ExplicitBuchiAutomata
translate_globally_atom(omega::core::APId ap, bool negated) {
  omega::automata::ExplicitBuchiAutomata a;

  auto q0 = a.add_state(true);
  a.add_initial(q0);

  auto g = guard_for_atom(ap, negated);
  a.add_edge(q0, q0, g);

  return a;
}

// F p  or  F !p
inline omega::automata::ExplicitBuchiAutomata
translate_finally_atom(omega::core::APId ap, bool negated) {
  omega::automata::ExplicitBuchiAutomata a;

  // q_wait: waiting until proposition becomes true
  // q_acc : proposition has happened, now accept forever
  auto q_wait = a.add_state(false);
  auto q_acc = a.add_state(true);

  a.add_initial(q_wait);

  // If target proposition holds move to accepting
  {
    auto g = guard_for_atom(ap, negated);
    a.add_edge(q_wait, q_acc, g);
  }

  // If target proposition does not hold remain waiting
  {
    auto g = guard_for_atom(ap, !negated);
    a.add_edge(q_wait, q_wait, g);
  }

  // Once satisfied stay accepting forever on any valuation
  a.add_edge(q_acc, q_acc);

  return a;
}

// p or !p holds at every position
// so done same as Globally for now
inline omega::automata::ExplicitBuchiAutomata
translate_atom_like(omega::core::APId ap, bool negated) {
  return translate_globally_atom(ap, negated);
}

} // namespace detail

inline omega::automata::ExplicitBuchiAutomata
translate_to_buchi(const FormulaPtr &f) {
  using K = FormulaKind;

  switch (f->kind()) {
  case K::Atom:
    return detail::translate_atom_like(f->atom(), false);

  case K::Not:
    if (f->left()->kind() == K::Atom) {
      return detail::translate_atom_like(f->left()->atom(), true);
    }
    break;

  case K::Finally:
    if (f->left()->kind() == K::Atom) {
      return detail::translate_finally_atom(f->left()->atom(), false);
    }
    if (f->left()->kind() == K::Not && f->left()->left()->kind() == K::Atom) {
      return detail::translate_finally_atom(f->left()->left()->atom(), true);
    }
    break;

  case K::Globally:
    if (f->left()->kind() == K::Atom) {
      return detail::translate_globally_atom(f->left()->atom(), false);
    }
    if (f->left()->kind() == K::Not && f->left()->left()->kind() == K::Atom) {
      return detail::translate_globally_atom(f->left()->left()->atom(), true);
    }
    break;

  default:
    break;
  }

  throw std::runtime_error("translate_to_buchi: unsupported formula fragment; "
                           "currently supports p, !p, F p, F !p, G p, G !p");
}

} // namespace ltl_mc
