#include <fstream>
#include <omega/automata/explicit_parity.hpp>
#include <omega/io/dot.hpp>

int main() {
  omega::automata::ExplicitParityAutomaton a;
  auto q0 = a.add_state();
  a.add_initial(q0);
  a.add_edge(q0, q0);
  a.set_priority(q0, 5);

  std::ofstream out("parity.dot");
  omega::io::write_dot(out, a, "parity");
  return 0;
}
