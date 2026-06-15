# tests — headless unit/validation tests. Links doctest + physics ONLY; never platform,
# GLFW, or glad, so CI runs them with no display or GPU (DESIGN §D6). This target also
# acts as the layering canary: it must build with no GL/windowing include paths.
add_executable(codephys_tests
    test_main.cpp
    test_math.cpp
)
target_link_libraries(codephys_tests
    PRIVATE
        doctest::doctest
        physics
        codephys::warnings)

add_test(NAME codephys_tests COMMAND codephys_tests)
