# app — the playground executable. Phase 0 runs the "hello triangle" smoke test.
# Depends on all module layers (physics, render, ui, platform); platform brings glfw +
# glad transitively so the throwaway triangle can issue GL calls (DESIGN §D3).
add_executable(codephys main.cpp)
target_link_libraries(codephys
    PRIVATE
        platform
        physics::math
        render
        ui
        codephys::warnings)
