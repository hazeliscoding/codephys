cmake_minimum_required(VERSION 3.21)

project(codephys
    VERSION 0.0.1
    DESCRIPTION "Cross-platform C++ physics simulation playground"
    LANGUAGES C CXX)

# --- Language standard -----------------------------------------------------------------
# C++20 everywhere, required, no compiler extensions — keeps MSVC/GCC/Clang in parity.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to RelWithDebInfo for single-config generators (Makefiles/Ninja). Multi-config
# generators (Visual Studio) pick the configuration at build time, so leave them alone.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

enable_testing()

# Strict warnings as a PRIVATE interface target, applied to first-party code only.
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/warnings.cmake)

# Third-party dependencies (GLFW fetched & pinned; glad + doctest vendored).
add_subdirectory(third_party)

# Module targets. Dependency direction is enforced by link/include visibility (DESIGN §5):
#   app -> {physics, render, ui, platform};  render -> physics::math;  ui -> render;
#   platform -> GLFW/glad;  physics -> stdlib only.
add_subdirectory(physics)
add_subdirectory(render)
add_subdirectory(ui)
add_subdirectory(platform)
add_subdirectory(app)
add_subdirectory(tests)
