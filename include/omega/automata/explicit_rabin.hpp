#pragma once

#include "omega/acceptance/rabin.hpp"
#include <omega/automata/omega_automaton.hpp>
#include <omega/ts/explicit_ts.hpp>

namespace omega::automata {
using ExplicitRabinAutomaton =
    OmegaAutomaton<omega::ts::ExplicitTS, omega::acceptance::Rabin>;
}
