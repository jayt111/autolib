#pragma once

#include <algorithm>
#include <cassert>
#include <omega/core/types.hpp>
#include <span>
#include <vector>

namespace omega::logic {

class Guard {
public:
  using APId = omega::core::APId;

  Guard() = default;

  static Guard true_guard() { return Guard{}; }

  bool empty() const noexcept { return positive_.empty() && negative_.empty(); }

  std::span<const APId> positive() const noexcept {
    return std::span<const APId>(positive_.data(), positive_.size());
  }

  std::span<const APId> negative() const noexcept {
    return std::span<const APId>(negative_.data(), negative_.size());
  }

  void require(APId ap) {
    assert(ap.valid());
    if (!contains(positive_, ap)) {
      positive_.push_back(ap);
    }
  }

  void forbid(APId ap) {
    assert(ap.valid());
    if (!contains(negative_, ap)) {
      negative_.push_back(ap);
    }
  }

  bool has_positive(APId ap) const noexcept { return contains(positive_, ap); }
  bool has_negative(APId ap) const noexcept { return contains(negative_, ap); }

  bool is_consistent() const noexcept {
    for (APId ap : positive_) {
      if (contains(negative_, ap)) {
        return false;
      }
    }
    return true;
  }

private:
  static bool contains(const std::vector<APId> &xs, APId ap) noexcept {
    return std::find(xs.begin(), xs.end(), ap) != xs.end();
  }
  std::vector<APId> positive_;
  std::vector<APId> negative_;
};
} // namespace omega::logic
