# render — hybrid 2D/3D renderer (DESIGN §7.2). Phase 0 stub. Depends downward only on
# physics::math (shared vector/matrix types); knows nothing of GLFW/windowing.
add_library(render STATIC src/render.cpp)
target_include_directories(render PUBLIC include)
target_link_libraries(render
    PUBLIC  physics::math
    PRIVATE codephys::warnings)
