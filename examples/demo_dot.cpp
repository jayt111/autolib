
#include <fstream>

#include <omega/algorithms/emptiness_buchi.hpp>
#include <omega/automata/explicit_buchi.hpp>
#include <omega/io/dot.hpp>

int main() {
  omega::automata::ExplicitBuchiAutomata a;

  auto q0 = a.add_state(false);
  auto q1 = a.add_state(true);

  a.add_initial(q0);
  a.add_edge(q0, q1);
  a.add_edge(q1, q1);

  auto r = omega::algorithms::buchi_emptiness(a, true);

  std::ofstream f("demo.dot");
  if (r.witness)
    omega::io::write_dot(f, a, *r.witness, "nba");
  else
    omega::io::write_dot(f, a, "nba");
}
