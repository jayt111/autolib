#pragma once

#include <omega/acceptance/parity.hpp>
#include <omega/automata/omega_automaton.hpp>
#include <omega/ts/explicit_ts.hpp>

namespace omega::automata {

using ExplicitParityAutomaton =
    OmegaAutomaton<omega::ts::ExplicitTS, omega::acceptance::Parity>;

}
