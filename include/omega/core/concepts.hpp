#pragma once

#include <concepts>
#include <cstddef>
#include <omega/core/types.hpp>
#include <ranges>
#include <span>
#include <type_traits>

namespace omega::core {

// Check a type is exactly StateId
template <class T>
concept StateIdLike = std::same_as<std::remove_cvref_t<T>, StateId>;

// Simple Transistion System concept
template <class TS>
concept TransitionSystem = requires(const TS ts, StateId s) {
  // Size
  { ts.num_states() } -> std::convertible_to<std::size_t>;

  // Initial States (multiple for NBA)
  // return std::span<const StateId> or any forward_range of StateId
  { ts.initial_states() } -> std::ranges::forward_range;
  requires StateIdLike<
      std::ranges::range_value_t<decltype(ts.initial_states())>>;

  // Outgoing edges view from a state
  // require it to be a span view (e.g. std::span<const Edge>)
  typename TS::edge_type;
  { ts.out_edges(s) } -> std::same_as<std::span<const typename TS::edge_type>>;
};

// Edge Concpets
// Must have destination
// minimal for now (maybe add labels)
template <class E>
concept HasDst = requires(const E e) { requires StateIdLike<decltype(e.dst)>; };

// ------------------ Base Omega Concept -------------------
template <class A>
concept OmegaAutomaton = TransitionSystem<A> && HasDst<typename A::edge_type>;

// ------------------ Automaton Concepts -------------------
// NOTE: Add other automaton here

// Buchi
template <class A>
concept BuchiAutomaton = OmegaAutomaton<A> && requires(const A a, StateId s) {
  { a.is_accepting(s) } -> std::convertible_to<bool>;
};

// Parity
template <class A>
concept ParityAutomaton = OmegaAutomaton<A> && requires(const A a, StateId s) {
  { a.priority(s) } -> std::convertible_to<Priority>;
};

// Rabin: pairs (E_i, F_i), accept iff ∃i: Inf(F_i) and Fin(E_i)
template <class A>
concept RabinAutomaton =
    OmegaAutomaton<A> && requires(const A a, std::size_t i, StateId s) {
      { a.num_pairs() } -> std::convertible_to<std::size_t>;
      { a.in_E(i, s) } -> std::convertible_to<bool>;
      { a.in_F(i, s) } -> std::convertible_to<bool>;
    };

// Streett: pairs (E_i, F_i), accept iff ∀i: Inf(E_i) => Inf(F_i)
template <class A>
concept StreettAutomaton =
    OmegaAutomaton<A> && requires(const A a, std::size_t i, StateId s) {
      { a.num_pairs() } -> std::convertible_to<std::size_t>;
      { a.in_E(i, s) } -> std::convertible_to<bool>;
      { a.in_F(i, s) } -> std::convertible_to<bool>;
    };

} // namespace omega::core
