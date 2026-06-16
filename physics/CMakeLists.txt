# physics — the custom physics library. STDLIB ONLY (CLAUDE.md hard rule):
# no GLFW/OpenGL/glad/render/ui include paths or link deps reach this target.
add_library(physics STATIC
    src/math.cpp
    src/integrator.cpp
    src/world.cpp
)
target_include_directories(physics PUBLIC include)
target_link_libraries(physics PRIVATE codephys::warnings)

# Name the dependency edge that upper layers are allowed to use: render/ui share only
# physics::math (DESIGN §5). In Phase 0 the math placeholder is all of physics, so the
# alias documents the seam and keeps call sites honest as physics grows.
add_library(physics::math ALIAS physics)
