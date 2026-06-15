# platform — owns the windowing/GL surface: GLFW + glad. Exposed PUBLIC so the app can
# issue the throwaway triangle GL calls (DESIGN §D3). No module other than `app` links
# platform, so GLFW/OpenGL never leak into render/ui/physics.
add_library(platform STATIC src/window.cpp)
target_include_directories(platform PUBLIC include)
target_link_libraries(platform
    PUBLIC  glfw glad
    PRIVATE codephys::warnings)
