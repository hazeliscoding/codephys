# app — the playground executable. Phase 1: fixed-timestep loop, time control, and the
# scene gallery (projectile + integrator comparison). Depends on all module layers.
add_executable(codephys
    main.cpp
    scene_manager.cpp
    scenes/ch03_projectile.cpp
    scenes/integrator_comparison.cpp
)
target_include_directories(codephys PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(codephys
    PRIVATE
        platform
        physics
        render
        ui
        codephys::warnings)
