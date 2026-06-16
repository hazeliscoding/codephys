# render — hybrid 2D/3D renderer (DESIGN §7.2). Phase 1: 2D primitives + Camera2D + text.
# Depends downward on physics::math (shared math types) and uses OpenGL internally via
# glad (the single glad instance is loaded once by platform). render exposes no GL types in
# its public headers, so upper layers stay GL-agnostic.
add_library(render STATIC src/renderer.cpp)
target_include_directories(render PUBLIC include)
target_link_libraries(render
    PUBLIC  physics::math
    PRIVATE glad codephys::warnings)
