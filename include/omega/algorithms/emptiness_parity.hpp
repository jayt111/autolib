#pragma once

// NOTE: Using SCC for this, might have to expand to Oink for alternating or
// tree but should be enough for now

#include "omega/algorithms/scc.hpp"
#include "omega/core/concepts.hpp"
#include "omega/core/types.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <queue>
#include <vector>
namespace omega::algorithms {

struct ParityLassoWitness {
  std::vector<omega::core::StateId> prefix;
  std::vector<omega::core::StateId> cycle;
  int scc_id = -1;
  omega::core::Priority min_priority = 0;
};

struct ParityEmptinessResult {
  bool non_empty = false;
  int witness_scc = -1;
  omega::core::Priority witness_min_priority = 0;
  std::optional<ParityLassoWitness> witness;
};

namespace detail {

inline bool is_even(omega::core::Priority p) noexcept { return (p % 2) == 0; }

inline std::vector<omega::core::StateId>
build_prefix_from_parents(omega::core::StateId goal,
                          const std::vector<omega::core::StateId> &parent) {
  using omega::core::StateId;
  std::vector<StateId> p;
  for (StateId cur = goal; cur.valid(); cur = parent[cur.raw()]) {
    p.push_back(cur);
  }
  std::reverse(p.begin(), p.end());
  if (p.empty())
    p.push_back(goal);
  return p;
}

template <omega::core::TransitionSystem TS>
inline bool has_self_loop(const TS &ts, omega::core::StateId s) {
  for (const auto &e : ts.out_edges(s)) {
    if (e.dst == s)
      return true;
  }
  return false;
}

} // namespace detail

template <omega::core::ParityAutomaton A>
ParityEmptinessResult parity_emptiness(const A &a, bool want_witness = false) {

  using omega::core::Priority;
  using omega::core::StateId;

  ParityEmptinessResult res;
  const std::size_t n = a.num_states();
  if (n == 0)
    return res;

  // -------------- Reachability from initials ---------------
  std::vector<std::uint8_t> reachable(n, 0);
  std::vector<StateId> parent(n, StateId::invalid());
  std::queue<StateId> q;

  for (StateId s : a.initial_states()) {
    if (!s.valid() || s.raw() >= n)
      continue;
    if (!reachable[s.raw()]) {
      reachable[s.raw()] = 1;
      parent[s.raw()] = StateId::invalid();
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

  bool any_reachable = false;
  for (auto r : reachable) {
    if (r) {
      any_reachable = true;
      break;
    }
  }
  if (!any_reachable)
    return res;

  // -------- SCC --------
  const auto scc = scc_tarjan(a);
  const int C = scc.num_components;
  if (C <= 0)
    return res;

  std::vector<std::uint8_t> comp_reachable(C, 0);
  std::vector<std::uint8_t> comp_has_selfloop(C, 0);
  std::vector<Priority> comp_min_prio(C, std::numeric_limits<Priority>::max());

  for (std::size_t i = 0; i < n; i++) {
    if (!reachable[i])
      continue;
    const int cid = scc.comp_of[i];
    if (cid < 0)
      continue;
    comp_reachable[cid] = 1;

    const Priority p =
        a.priority(StateId{static_cast<StateId::underlying_type>(i)});
    comp_min_prio[cid] = std::min(comp_min_prio[cid], p);
  }

  for (std::size_t i = 0; i < n; i++) {
    const int cid = scc.comp_of[i];
    if (cid < 0)
      continue;
    StateId v{static_cast<StateId::underlying_type>(i)};
    if (detail::has_self_loop(a, v))
      comp_has_selfloop[cid] = 1;
  }

  // ------ Witness ------
  int witness_cid = -1;
  Priority witness_min = 0;

  for (int cid = 0; cid < C; cid++) {
    if (!comp_reachable[cid])
      continue;
    const auto &comp_states = scc.components[cid];
    const bool is_cyclic =
        (comp_states.size() > 1 ||
         (comp_states.size() == 1 && comp_has_selfloop[cid]));

    if (!is_cyclic)
      continue;

    const Priority mp = comp_min_prio[cid];
    if (mp == std::numeric_limits<Priority>::max())
      continue;
    if (!detail::is_even(mp))
      continue;

    witness_cid = cid;
    witness_min = mp;
    break;
  }

  if (witness_cid < 0)
    return res;

  res.non_empty = true;
  res.witness_scc = witness_cid;
  res.witness_min_priority = witness_min;

  if (!want_witness)
    return res;

  // ------- Lasso -------
  ParityLassoWitness w;
  w.scc_id = witness_cid;
  w.min_priority = witness_min;

  const auto &comp_states = scc.components[witness_cid];

  // Cycle entry as state with min priority
  StateId cycle_start = StateId::invalid();
  for (StateId s : comp_states) {
    if (a.priority(s) == witness_min) {
      cycle_start = s;
      break;
    }
  }
  if (!cycle_start.valid())
    return res;

  w.prefix = detail::build_prefix_from_parents(cycle_start, parent);

  // must self-loop
  if (comp_states.size() == 1) {
    if (!detail::has_self_loop(a, cycle_start))
      return res;
    w.cycle = {cycle_start, cycle_start};
    res.witness = std::move(w);
    return res;
  }

  // find pred in SCC with edge pred -> cycle_start
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
  if (!pred.valid())
    return res;

  // BFS inside SCC from cycle_start to pred
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
      if (nxt.raw() >= n)
        continue;
      if (scc.comp_of[nxt.raw()] != witness_cid)
        continue;
      if (!seen[nxt.raw()]) {
        seen[nxt.raw()] = 1;
        p2[nxt.raw()] = v;
        q2.push(nxt);
      }
    }
  }
  if (!seen[pred.raw()])
    return res;

  // reconstruct cycle_start to pred
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
  if (path.empty() || path.front() != cycle_start || path.back() != pred)
    return res;

  w.cycle = std::move(path);
  w.cycle.push_back(cycle_start); // close
  res.witness = std::move(w);
  return res;
} // end fun

} // namespace omega::algorithms
