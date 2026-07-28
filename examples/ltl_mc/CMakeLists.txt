option(LTL_BUILD_TESTS "Build Tests" ON)

add_executable(ltl_mc
  src/main.cpp
)

target_include_directories(ltl_mc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(ltl_mc PRIVATE omega)
target_compile_features(ltl_mc PRIVATE cxx_std_20)


if (LTL_BUILD_TESTS)
  enable_testing()
  function(ltl_add_test test_name test_src)
    add_executable(${test_name} ${test_src})
    target_include_directories(${test_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(${test_name} PRIVATE omega)
    add_test(NAME ${test_name} COMMAND ${test_name})
  endfunction()

  ltl_add_test(test_formula tests/test_formula.cpp)
  ltl_add_test(test_negate tests/test_negate.cpp)
  ltl_add_test(test_translate tests/test_translate.cpp)
  ltl_add_test(test_model_check tests/test_model_check.cpp)

endif()



