#include <fstream>
#include <iostream>

#include "formula.hpp"
#include "model_check.hpp"
#include "negate.hpp"
#include "print.hpp"
#include "translate_to_buchi.hpp"

#include <omega/io/dot.hpp>
#include <omega/io/dot_kripke.hpp>
#include <omega/io/dot_product.hpp>
#include <omega/kripke/explicit_kripke.hpp>
#include <omega/kripke/product_kripke_buchi.hpp>

namespace {

void print_state_path(const char *name,
                      const std::vector<omega::core::StateId> &path) {
  std::cout << name << ": ";
  for (std::size_t i = 0; i < path.size(); ++i) {
    if (i > 0) {
      std::cout << " -> ";
    }
    std::cout << "s" << path[i].raw();
  }
  std::cout << '\n';
}

} // namespace

int main() {
  using ltl_mc::Formula;
  using omega::core::APId;

  // Build a small Kripke structure
  omega::kripke::ExplicitKripke k;

  APId p = k.add_atomic_proposition();

  auto s0 = k.add_state();
  auto s1 = k.add_state();

  k.add_initial(s0);
  k.add_edge(s0, s1);
  k.add_edge(s1, s1);

  k.set_holds(s0, p, false);
  k.set_holds(s1, p, true);

  auto phi = Formula::make_globally(Formula::make_atom(p));

  std::cout << "Checking formula: ";
  ltl_mc::print_formula(std::cout, phi);
  std::cout << "\n";

  // Build the standard automata-based model-checking pipeline
  auto neg_phi = ltl_mc::negate(phi);

  std::cout << "Negated formula: ";
  ltl_mc::print_formula(std::cout, neg_phi);
  std::cout << "\n";

  auto ba = ltl_mc::translate_to_buchi(neg_phi);
  auto product = omega::kripke::product_kripke_buchi(k, ba);
  auto result = ltl_mc::model_check(k, phi);

  // Print result
  if (result.holds) {
    std::cout << "Result: property holds.\n";
  } else {
    std::cout << "Result: property does NOT hold.\n";

    if (result.kripke_counterexample.has_value()) {
      const auto &w = *result.kripke_counterexample;
      print_state_path("Counterexample prefix", w.prefix);
      print_state_path("Counterexample cycle ", w.cycle);
    }
  }

  // Write DOT files
  {
    std::ofstream out("kripke.dot");
    omega::io::write_dot(out, k, "kripke");
  }

  {
    std::ofstream out("neg_phi_buchi.dot");
    omega::io::write_dot(out, ba, "neg_phi_buchi");
  }

  {
    std::ofstream out("product.dot");
    omega::io::write_dot(out, product, "product");
  }

  if (result.product_counterexample.has_value()) {
    std::ofstream out("product_witness.dot");
    omega::io::write_dot(out, product, *result.product_counterexample,
                         "product_witness");
  }

  std::cout << "Wrote DOT files:\n";
  std::cout << "  kripke.dot\n";
  std::cout << "  neg_phi_buchi.dot\n";
  std::cout << "  product.dot\n";
  if (result.product_counterexample.has_value()) {
    std::cout << "  product_witness.dot\n";
  }

  return 0;
}
