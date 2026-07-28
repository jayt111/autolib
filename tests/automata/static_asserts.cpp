#include "omega/automata/explicit_buchi.hpp"
#include "omega/automata/explicit_parity.hpp"
#include "omega/automata/explicit_rabin.hpp"
#include <omega/core/concepts.hpp>

static_assert(
    omega::core::BuchiAutomaton<omega::automata::ExplicitBuchiAutomata>);

static_assert(
    omega::core::ParityAutomaton<omega::automata::ExplicitParityAutomaton>);

static_assert(
    omega::core::RabinAutomaton<omega::automata::ExplicitRabinAutomaton>);
