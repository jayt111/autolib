#pragma once

#include <ostream>

#include "formula.hpp"

namespace ltl_mc {

inline void print_formula(std::ostream &os, const FormulaPtr &f) {
  using K = FormulaKind;

  switch (f->kind()) {
  case K::True:
    os << "true";
    break;
  case K::False:
    os << "false";
    break;
  case K::Atom:
    os << "p" << f->atom().raw();
    break;
  case K::Not:
    os << "!(";
    print_formula(os, f->left());
    os << ")";
    break;
  case K::And:
    os << "(";
    print_formula(os, f->left());
    os << " && ";
    print_formula(os, f->right());
    os << ")";
    break;
  case K::Or:
    os << "(";
    print_formula(os, f->left());
    os << " || ";
    print_formula(os, f->right());
    os << ")";
    break;
  case K::Next:
    os << "X(";
    print_formula(os, f->left());
    os << ")";
    break;
  case K::Finally:
    os << "F(";
    print_formula(os, f->left());
    os << ")";
    break;
  case K::Globally:
    os << "G(";
    print_formula(os, f->left());
    os << ")";
    break;
  case K::Until:
    os << "(";
    print_formula(os, f->left());
    os << " U ";
    print_formula(os, f->right());
    os << ")";
    break;
  case K::Release:
    os << "(";
    print_formula(os, f->left());
    os << " R ";
    print_formula(os, f->right());
    os << ")";
    break;
  }
}

} // namespace ltl_mc
