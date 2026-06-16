#include <platform/window.hpp>

// glad provides the GL symbols; GLFW_INCLUDE_NONE stops GLFW from pulling in its own GL
// header (which would clash with glad). With it defined, include order is irrelevant.
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <cstdio>
#include <stdexcept>
#include <string>

namespace platform {
namespace {

void glfw_error_callback(int code, const char* description) {
    std::fprintf(stderr, "[GLFW error %d] %s\n", code, description ? description : "");
}

void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void scroll_callback(GLFWwindow* window, double /*x_offset*/, double y_offset) {
    if (auto* accum = static_cast<double*>(glfwGetWindowUserPointer(window))) {
        *accum += y_offset;
    }
}

std::string gl_string(GLenum name) {
    const GLubyte* value = glGetString(name);
    return value ? reinterpret_cast<const char*>(value) : "(null)";
}

}  // namespace

Window::Window(int width, int height, const std::string& title) {
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        throw std::runtime_error("GLFW initialization failed");
    }

    // Request an OpenGL 3.3 Core profile context (DESIGN §3 / §D4).
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                   GLFW_TRUE);  // required on macOS, harmless elsewhere

    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error(
            "Failed to create a GLFW window with an OpenGL 3.3 Core context");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // vsync: present each frame without busy-spinning

    if (!gladLoadGL()) {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw std::runtime_error("Failed to load OpenGL function pointers via glad");
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowUserPointer(window, &scroll_accum_);
    glfwSetScrollCallback(window, scroll_callback);
    handle_ = window;
}

Window::~Window() {
    if (handle_) {
        glfwDestroyWindow(static_cast<GLFWwindow*>(handle_));
    }
    glfwTerminate();
}

void Window::run(const RenderCallback& render) {
    auto* window = static_cast<GLFWwindow*>(handle_);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        int fb_width = 0;
        int fb_height = 0;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);

        if (render) {
            render(fb_width, fb_height);
        }

        glfwSwapBuffers(window);
    }
}

void Window::mouse_position(double& x, double& y) const {
    auto* window = static_cast<GLFWwindow*>(handle_);
    double cursor_x = 0.0;
    double cursor_y = 0.0;
    glfwGetCursorPos(window, &cursor_x, &cursor_y);
    int win_w = 0, win_h = 0, fb_w = 0, fb_h = 0;
    glfwGetWindowSize(window, &win_w, &win_h);
    glfwGetFramebufferSize(window, &fb_w, &fb_h);
    // Scale window coords to framebuffer pixels (matters under HiDPI).
    x = win_w > 0 ? cursor_x * static_cast<double>(fb_w) / win_w : cursor_x;
    y = win_h > 0 ? cursor_y * static_cast<double>(fb_h) / win_h : cursor_y;
}

bool Window::mouse_left_down() const {
    auto* window = static_cast<GLFWwindow*>(handle_);
    return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
}

bool Window::key_down(int key) const {
    auto* window = static_cast<GLFWwindow*>(handle_);
    return glfwGetKey(window, key) == GLFW_PRESS;
}

double Window::take_scroll() {
    const double s = scroll_accum_;
    scroll_accum_ = 0.0;
    return s;
}

GlInfo query_gl_info() {
    return GlInfo{
        gl_string(GL_VERSION),
        gl_string(GL_RENDERER),
        gl_string(GL_SHADING_LANGUAGE_VERSION),
    };
}

double now_seconds() {
    return glfwGetTime();
}

bool check_gl_errors(const char* where) {
    bool clean = true;
    for (GLenum err = glGetError(); err != GL_NO_ERROR; err = glGetError()) {
        std::fprintf(stderr, "GL error 0x%04X at %s\n", err, where ? where : "?");
        clean = false;
    }
    return clean;
}

}  // namespace platform
