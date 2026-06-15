#pragma once

#include <functional>
#include <string>

namespace platform {

// Per-frame render callback: invoked once per loop iteration with the current framebuffer
// size in pixels. All drawing for the frame happens here.
using RenderCallback = std::function<void(int width, int height)>;

// RAII window + OpenGL 3.3 Core context (GLFW-backed, glad-loaded).
//
// Construction initializes GLFW, creates the window/context, makes it current, and loads
// GL function pointers. On any failure it throws std::runtime_error with a clear
// diagnostic; callers translate that into a non-zero exit. Destruction tears everything
// down cleanly. This is the only place (besides the app's throwaway triangle) that
// touches GLFW/OpenGL, so the GL headers never leak through this interface — the native
// handle is kept opaque.
class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    // Run the event loop until the user closes the window or presses Escape. Each frame:
    // poll events, invoke `render`, swap buffers.
    void run(const RenderCallback& render);

private:
    void* handle_ = nullptr;  // GLFWwindow*, opaque so this header needs no GLFW include
};

// OpenGL/driver strings captured from the current context — for cross-platform and
// WSLg/software-GL diagnostics.
struct GlInfo {
    std::string version;
    std::string renderer;
    std::string glsl_version;
};

// Query the current context's GL info. Requires a current context (i.e. a live Window).
GlInfo query_gl_info();

}  // namespace platform
