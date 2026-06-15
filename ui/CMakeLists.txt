# ui — hand-rolled immediate-mode UI (DESIGN §7.3). Phase 0 stub. Draws through the
# renderer, so it sees `render` (and physics::math transitively) and nothing else.
add_library(ui STATIC src/ui.cpp)
target_include_directories(ui PUBLIC include)
target_link_libraries(ui
    PUBLIC  render
    PRIVATE codephys::warnings)
