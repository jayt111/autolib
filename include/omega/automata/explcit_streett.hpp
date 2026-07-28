#pragma once

#include "omega/acceptance/streett.hpp"
#include <omega/automata/omega_automaton.hpp>
#include <omega/ts/explicit_ts.hpp>

namespace omega::automata {
using ExplicitStreettAutomaton =
    OmegaAutomaton<omega::ts::ExplicitTS, omega::acceptance::Streett>;
}
