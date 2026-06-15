# Third-party dependencies.
#
# Policy (CLAUDE.md): GLFW is the ONLY runtime dependency. glad and doctest are vendored,
# build/test-time only. None of these targets link codephys::warnings — their warnings
# must never fail our build.

include(FetchContent)

# --- GLFW 3.4 (runtime dependency, fetched and pinned) ---------------------------------
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
# X11 only on Linux: WSLg renders via Xwayland, and this keeps the Linux dependency set
# to just libgl1-mesa-dev + xorg-dev (no wayland-scanner / libwayland-dev needed), which
# matches the CI install step. Re-enable Wayland later if a native-Wayland target is wanted.
set(GLFW_BUILD_WAYLAND  OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11      ON  CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4            # pinned stable tag
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(glfw)

# --- glad (vendored generated GL 3.3 Core loader) --------------------------------------
add_library(glad STATIC glad/src/glad.c)
# SYSTEM so consumers (platform, app) don't inherit glad's header warnings under /WX.
target_include_directories(glad SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)
# glad's self-contained loader dlopen's the GL library on Linux; needs libdl there.
target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})

# --- doctest (vendored single header, test-time only) ----------------------------------
add_library(doctest INTERFACE)
add_library(doctest::doctest ALIAS doctest)
# Header is at doctest/doctest.h, so consumers write #include <doctest/doctest.h>.
# SYSTEM so doctest's header warnings don't trip /WX in our test TUs.
target_include_directories(doctest SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
