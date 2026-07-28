#pragma once

#include "omega/core/concepts.hpp"
#include "omega/core/types.hpp"
#include <algorithm>
#include <cstddef>
#include <omega/algorithms/scc.hpp>
#include <queue>
#include <vector>
namespace omega::algorithms {

struct BuchiLassoWitness {
  // path from some initial state to the cycle entry
  std::vector<omega::core::StateId> prefix;

  // Closed walk: cycle.front() == cycle.back()
  std::vector<omega::core::StateId> cycle;

  int scc_id = -1;
};

struct BuchiEmptinessResult {
  bool non_empty = false;

  // if non_empty, one accepting SCC id that witnesses non emptiness
  int witness_scc = -1;

  // present if requested and non_empty
  std::optional<BuchiLassoWitness> witness;
};

namespace detail {

// Reconstruct path start to goal using parent pointers
inline std::vector<omega::core::StateId>
reconstruct_path(omega::core::StateId start, omega::core::StateId goal,
                 const std::vector<omega::core::StateId> &parent) {
  using omega::core::StateId;

  std::vector<StateId> path;
  StateId cur = goal;
  while (cur.valid()) {
    path.push_back(cur);
    if (cur == start)
      break;
    cur = parent[cur.raw()];
  }
  std::reverse(path.begin(), path.end());
  return path;
}

// Pick an accepitng state from SCC (as cycle start)
template <omega::core::BuchiAutomaton A>
inline omega::core::StateId
pick_accepting_in_scc(const A &a,
                      const std::vector<omega::core::StateId> &comp) {
  for (auto s : comp) {
    if (a.is_accepting(s))
      return s;
  }
  return omega::core::StateId::invalid();
}

template <omega::core::BuchiAutomaton A>
inline bool has_self_loop(const A &a, omega::core::StateId s) {
  for (const auto &e : a.out_edges(s)) {
    if (e.dst == s)
      return true;
  }
  return false;
}

} // namespace detail

template <omega::core::BuchiAutomaton A>
BuchiEmptinessResult buchi_emptiness(const A &a, bool want_witness = false) {
  using omega::core::StateId;

  const std::size_t n = a.num_states();
  BuchiEmptinessResult res;

  if (n == 0)
    return res;

  // Reachability from inital states
  std::vector<std::uint8_t> reachable(n, 0);
  std::vector<StateId> parent(n, StateId::invalid());
  std::queue<StateId> q;

  for (StateId s : a.initial_states()) {
    if (!s.valid() || s.raw() >= n)
      continue;
    if (!reachable[s.raw()]) {
      reachable[s.raw()] = 1;
      parent[s.raw()] = StateId::invalid(); // root marker
      q.push(s);
    }
  }

  while (!q.empty()) {
    StateId v = q.front();
    q.pop();
    for (const auto &e : a.out_edges(v)) {
      StateId w = e.dst;
      if (w.raw() >= n)
        continue;
      if (!reachable[w.raw()]) {
        reachable[w.raw()] = 1;
        parent[w.raw()] = v;
        q.push(w);
      }
    }
  }

  // If nothing reachable, then empty
  bool any_reachable = false;
  for (auto r : reachable) {
    if (r) {
      any_reachable = true;
      break;
    }
  }

  if (!any_reachable)
    return res;

  // SCC
  auto scc = scc_tarjan(a);

  // identify sccs that are reachable, accepting and cyclic
  const int C = scc.num_components;
  if (C <= 0)
    return res;

  std::vector<std::uint8_t> comp_reachable(C, 0);
  std::vector<std::uint8_t> comp_has_accepting(C, 0);
  std::vector<std::uint8_t> comp_has_selfloop(C, 0);

  // mark reachable and accepting per component
  for (std::size_t i = 0; i < n; i++) {
    if (!reachable[i])
      continue;

    const int cid = scc.comp_of[i];
    if (cid < 0)
      continue;

    comp_reachable[cid] = 1;
    if (a.is_accepting(StateId{static_cast<StateId::underlying_type>(i)})) {
      comp_has_accepting[cid] = 1;
    }
  }

  // detect self loop
  for (std::size_t i = 0; i < n; i++) {
    const int cid = scc.comp_of[i];
    if (cid < 0)
      continue;

    StateId v{static_cast<StateId::underlying_type>(i)};
    if (detail::has_self_loop(a, v))
      comp_has_selfloop[cid] = 1;
  }

  // Check each SCC
  int witnesses_cid = -1;
  for (int cid = 0; cid < C; cid++) {
    if (!comp_reachable[cid])
      continue;
    if (!comp_has_accepting[cid])
      continue;

    const auto &comp_states = scc.components[cid];
    const bool is_cyclic = (comp_states.size() > 1) ||
                           (comp_states.size() == 1 && comp_has_selfloop[cid]);

    if (is_cyclic) {
      witnesses_cid = cid;
      break;
    }
  }
  if (witnesses_cid < 0)
    return res;

  res.non_empty = true;
  res.witness_scc = witnesses_cid;

  if (!want_witness)
    return res;

  // build lasso witness
  BuchiLassoWitness w;
  w.scc_id = witnesses_cid;

  const auto &comp_states = scc.components[witnesses_cid];

  // Choose accepting state in SCC as cycle entry
  StateId cycle_start = detail::pick_accepting_in_scc(a, comp_states);
  if (!cycle_start.valid()) {
    // Shoudlnt happen
    res.witness.reset();
    return res;
  }

  // Build prefix
  // walk parents back to a root then reconstruct
  StateId root = cycle_start;
  while (true) {
    StateId p = parent[root.raw()];
    if (!p.valid())
      break;
    root = p;
  }

  w.prefix = detail::reconstruct_path(root, cycle_start, parent);
  if (w.prefix.empty() || w.prefix.back() != cycle_start) {
    // Fallback: minimal prefix
    w.prefix = {cycle_start};
  }

  // Build a cycle inside SCC returning to cycle_start
  if (comp_states.size() == 1) {
    // Must be a slef loop
    if (!detail::has_self_loop(a, cycle_start)) {
      res.witness.reset();
      return res;
    }
    w.cycle = {cycle_start, cycle_start};
  } else {
    // FInd a predecessor in SCC with an edge pred -> cycle_start
    StateId pred = StateId::invalid();
    for (StateId u : comp_states) {
      for (const auto &e : a.out_edges(u)) {
        if (e.dst == cycle_start) {
          pred = u;
          break;
        }
      }
      if (pred.valid())
        break;
    }
    if (!pred.valid()) {
      res.witness.reset();
      return res;
    }

    // BFS within SCC to find path cycle_start -> pred
    std::vector<std::uint8_t> seen(n, 0);
    std::vector<StateId> p2(n, StateId::invalid());
    std::queue<StateId> q2;

    seen[cycle_start.raw()] = 1;
    q2.push(cycle_start);

    while (!q2.empty() && !seen[pred.raw()]) {
      StateId v = q2.front();
      q2.pop();
      for (const auto &e : a.out_edges(v)) {
        StateId nxt = e.dst;
        if (nxt.raw() >= n || scc.comp_of[nxt.raw()] != witnesses_cid)
          continue;
        if (!seen[nxt.raw()]) {
          seen[nxt.raw()] = 1;
          p2[nxt.raw()] = v;
          q2.push(nxt);
        }
      }
    }
    if (!seen[pred.raw()]) {
      res.witness.reset();
      return res;
    }

    // Reconstruct path cycle_start -> pred
    std::vector<StateId> path;
    {
      StateId cur = pred;
      while (cur.valid()) {
        path.push_back(cur);
        if (cur == cycle_start)
          break;
        cur = p2[cur.raw()];
      }
      std::reverse(path.begin(), path.end());
    }

    if (path.empty() || path.front() != cycle_start || path.back() != pred) {
      res.witness.reset();
      return res;
    }
    w.cycle = std::move(path);
    w.cycle.push_back(cycle_start);
  }

  res.witness = std::move(w);
  return res;
}
// TODO: Add helper functions for bool accepting and lasso, (spilt big function
// with two seperate fun calls)

} // namespace omega::algorithms
