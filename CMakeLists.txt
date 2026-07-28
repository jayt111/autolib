cmake_minimum_required(VERSION 3.20)

project(omega_automata
  VERSION 0.1.0
  LANGUAGES CXX
)

# Options
option(OMEGA_BUILD_EXAMPLES "Build examples" ON)
option(OMEGA_BUILD_TESTS    "Build tests"    ON)
option(OMEGA_BUILD_SHARED   "Build as shared library" OFF)

# --- Library target ---
set(OMEGA_LIB_TYPE STATIC)
if (OMEGA_BUILD_SHARED)
  set(OMEGA_LIB_TYPE SHARED)
endif()

add_library(omega ${OMEGA_LIB_TYPE})

target_sources(omega PRIVATE
  src/omega.cpp
)

target_include_directories(omega
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# C++20
target_compile_features(omega PUBLIC cxx_std_20)

# Warnings
if (MSVC)
  target_compile_options(omega PRIVATE /W4 /permissive-)
else()
  target_compile_options(omega PRIVATE -Wall -Wextra -Wpedantic)
endif()

# --- Examples ---
if (OMEGA_BUILD_EXAMPLES)
  add_subdirectory(examples/ltl_mc)
  # Helper function to add a example
  function(omega_add_example ex_name ex_src)
    add_executable(${ex_name} ${ex_src})
    target_link_libraries(${ex_name} PRIVATE omega)
  endfunction()

  omega_add_example(demo_dot examples/demo_dot.cpp)
  omega_add_example(dot_parity_demo examples/parity_dot.cpp)


endif()

# --- Tests ---
if (OMEGA_BUILD_TESTS)
  enable_testing()

  # Helper function to add a test executable + register it with ctest
  function(omega_add_test test_name test_src)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE omega)
    add_test(NAME ${test_name} COMMAND ${test_name})
  endfunction()

  omega_add_test(test_types          tests/core/test_types.cpp)
  omega_add_test(test_explicit_buchi tests/automata/test_explicit_buchi.cpp)
  omega_add_test(test_scc tests/algorithms/test_scc.cpp)
  omega_add_test(test_emptiness_buchi tests/algorithms/test_emptiness_buchi.cpp)
  omega_add_test(test_emptiness_buchi_witness tests/algorithms/test_emptiness_buchi_witness.cpp)
  omega_add_test(test_explicit_parity tests/automata/test_explicit_parity.cpp)
  omega_add_test(test_parity_acceptance tests/acceptance/test_parity.cpp)
  omega_add_test(test_rabin_acc tests/acceptance/test_rabin_acc.cpp)
  omega_add_test(test_explicit_rabin tests/automata/test_explicit_rabin.cpp)
  omega_add_test(test_emptiness_parity tests/algorithms/test_emptiness_parity.cpp)
  omega_add_test(test_rabin_to_parity tests/algorithms/test_rabin_to_parity.cpp)
  omega_add_test(test_product_buchi tests/algorithms/test_product_buchi.cpp)
  omega_add_test(test_explicit_kripke tests/kripke/test_explicit_kripke.cpp)
  omega_add_test(test_guard tests/logic/test_guard.cpp)
  omega_add_test(test_buchi_guards tests/automata/test_buchi_guards.cpp)
  omega_add_test(test_product_kripke_buchi tests/kripke/test_product_kripke_buchi.cpp)

endif()
