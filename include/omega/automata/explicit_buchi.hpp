#pragma once

#include <omega/acceptance/buchi.hpp>
#include <omega/automata/omega_automaton.hpp>
#include <omega/ts/explicit_ts.hpp>

namespace omega::automata {

using ExplicitBuchiAutomata =
    OmegaAutomaton<omega::ts::ExplicitTS, omega::acceptance::Buchi>;

} // namespace omega::automata
