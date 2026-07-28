#pragma once

#include <cassert>
#include <memory>
#include <utility>

#include <omega/core/types.hpp>

namespace ltl_mc {

enum class FormulaKind {
  True,
  False,
  Atom,
  Not,
  And,
  Or,
  Next,
  Finally,
  Globally,
  Until,
  Release
};

class Formula;
using FormulaPtr = std::shared_ptr<Formula>;

class Formula {
public:
  using APId = omega::core::APId;

  FormulaKind kind() const noexcept { return kind_; }

  APId atom() const noexcept {
    assert(kind_ == FormulaKind::Atom);
    return atom_;
  }

  const FormulaPtr &left() const noexcept {
    assert(left_);
    return left_;
  }

  const FormulaPtr &right() const noexcept {
    assert(right_);
    return right_;
  }

  static FormulaPtr make_true() {
    return std::shared_ptr<Formula>(new Formula(FormulaKind::True));
  }

  static FormulaPtr make_false() {
    return std::shared_ptr<Formula>(new Formula(FormulaKind::False));
  }

  static FormulaPtr make_atom(APId ap) {
    return std::shared_ptr<Formula>(new Formula(ap));
  }

  static FormulaPtr make_not(FormulaPtr x) {
    return std::shared_ptr<Formula>(
        new Formula(FormulaKind::Not, std::move(x), nullptr));
  }

  static FormulaPtr make_and(FormulaPtr l, FormulaPtr r) {
    return std::shared_ptr<Formula>(
        new Formula(FormulaKind::And, std::move(l), std::move(r)));
  }

  static FormulaPtr make_or(FormulaPtr l, FormulaPtr r) {
    return std::shared_ptr<Formula>(
        new Formula(FormulaKind::Or, std::move(l), std::move(r)));
  }

  static FormulaPtr make_next(FormulaPtr x) {
    return std::shared_ptr<Formula>(
        new Formula(FormulaKind::Next, std::move(x), nullptr));
  }

  static FormulaPtr make_finally(FormulaPtr x) {
    return std::shared_ptr<Formula>(
        new Formula(FormulaKind::Finally, std::move(x), nullptr));
  }

  static FormulaPtr make_globally(FormulaPtr x) {
    return std::shared_ptr<Formula>(
        new Formula(FormulaKind::Globally, std::move(x), nullptr));
  }

  static FormulaPtr make_until(FormulaPtr l, FormulaPtr r) {
    return std::shared_ptr<Formula>(
        new Formula(FormulaKind::Until, std::move(l), std::move(r)));
  }

  static FormulaPtr make_release(FormulaPtr l, FormulaPtr r) {
    return std::shared_ptr<Formula>(
        new Formula(FormulaKind::Release, std::move(l), std::move(r)));
  }

private:
  explicit Formula(FormulaKind kind) : kind_(kind) {}

  explicit Formula(APId ap) : kind_(FormulaKind::Atom), atom_(ap) {}

  Formula(FormulaKind kind, FormulaPtr l, FormulaPtr r)
      : kind_(kind), left_(std::move(l)), right_(std::move(r)) {}

  FormulaKind kind_;
  APId atom_{APId::invalid()};
  FormulaPtr left_{};
  FormulaPtr right_{};
};

} // namespace ltl_mc
