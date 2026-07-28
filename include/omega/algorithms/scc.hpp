#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <omega/core/concepts.hpp>
#include <omega/core/types.hpp>
#include <vector>

namespace omega::algorithms {

struct SccResult {
  // component id per state
  std::vector<int> comp_of;

  int num_components = 0;

  // components[c] = list of states in SCC c
  std::vector<std::vector<omega::core::StateId>> components;
};

// Tarjan SCC algorithm
template <omega::core::TransitionSystem TS> SccResult scc_tarjan(const TS &ts) {
  using omega::core::StateId;

  const std::size_t n = ts.num_states();
  SccResult out;
  out.comp_of.assign(n, -1);

  std::vector<int> index(n, -1);
  std::vector<int> lowlink(n, -1);
  std::vector<std::uint8_t> on_stack(n, 0);

  std::vector<StateId> stack;
  stack.reserve(n);

  int next_index = 0;

  auto strongconnect = [&](auto &&self, StateId v) -> void {
    const std::size_t vi = v.raw();
    index[vi] = next_index;
    lowlink[vi] = next_index;
    ++next_index;

    stack.push_back(v);
    on_stack[vi] = 1;

    for (const auto &e : ts.out_edges(v)) {
      const StateId w = e.dst;
      const std::size_t wi = w.raw();

      if (index[wi] == -1) {
        self(self, w);
        lowlink[vi] = std::min(lowlink[vi], lowlink[wi]);
      } else if (on_stack[wi]) {
        lowlink[vi] = std::min(lowlink[vi], index[wi]);
      }
    }

    // If v is a root node, pop stack and generate SCC
    if (lowlink[vi] == index[vi]) {
      const int comp_id = out.num_components++;
      out.components.emplace_back();

      while (true) {
        StateId w = stack.back();
        stack.pop_back();
        on_stack[w.raw()] = 0;

        out.comp_of[w.raw()] = comp_id;
        out.components.back().push_back(w);

        if (w == v)
          break;
      }
    }
  };

  for (std::size_t i = 0; i < n; i++) {
    StateId v{static_cast<StateId::underlying_type>(i)};
    if (index[i] == -1) {
      strongconnect(strongconnect, v);
    }
  }

  return out;
}

} // namespace omega::algorithms
