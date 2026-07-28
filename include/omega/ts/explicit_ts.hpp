#pragma once

#include <cassert>
#include <omega/core/types.hpp>
#include <omega/logic/guard.hpp>
#include <optional>
#include <span>
#include <vector>

namespace omega::ts {

class ExplicitTS {
public:
  using StateId = omega::core::StateId;

  struct edge_type {
    StateId dst;
    std::optional<omega::logic::Guard> guard;
  };

  ExplicitTS() = default;

  std::size_t num_states() const noexcept { return adj_.size(); }

  std::span<const StateId> initial_states() const noexcept {
    return std::span<const StateId>(initials_.data(), initials_.size());
  }

  std::span<const edge_type> out_edges(StateId s) const noexcept {
    assert_state(s);
    const auto &v = adj_[s.raw()];
    return std::span<const edge_type>(v.data(), v.size());
  }

  StateId add_state() {
    const std::size_t idx = adj_.size();
    assert(idx < StateId::invalid_value() &&
           "Too many states for StateId underlying type");

    adj_.emplace_back(); // empty outgoing edge list
    return StateId(static_cast<StateId::underlying_type>(idx));
  }

  void reserve_states(std::size_t n) { adj_.reserve(n); }

  void add_initial(StateId s) {
    assert_state(s);
    initials_.push_back(s);
  }

  void clear_initials() noexcept { initials_.clear(); }

  void add_edge(StateId src, StateId dst) {
    assert_state(src);
    assert_state(dst);
    adj_[src.raw()].push_back(edge_type{dst, std::nullopt});
  }

  void add_edge(StateId src, StateId dst, const omega::logic::Guard &guard) {
    assert_state(src);
    assert_state(dst);
    adj_[src.raw()].push_back(edge_type{dst, guard});
  }

  // conveincence function, return reference to an edge when adding it
  edge_type &add_edge_ref(StateId src, StateId dst) {
    assert_state(src);
    assert_state(dst);
    auto &vec = adj_[src.raw()];
    vec.push_back(edge_type{dst, std::nullopt});
    return vec.back();
  }

private:
  void assert_state(StateId s) const noexcept {
    assert(s.valid());
    assert(s.raw() < adj_.size());
  }

  std::vector<std::vector<edge_type>> adj_;
  std::vector<StateId> initials_;
};

} // namespace omega::ts
