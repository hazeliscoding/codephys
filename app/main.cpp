// CodePhys — Phase 0 "hello triangle" smoke test.
//
// Proves the cross-platform GL pipeline end to end: window + OpenGL 3.3 Core context
// (platform::Window), shader compilation, a VBO/VAO upload, and a draw call. This is a
// throwaway test, NOT the Phase 1 renderer (DESIGN §10, Phase 0).

#include <glad/glad.h>
#include <platform/window.hpp>

#include <cstdio>
#include <cstdlib>
#include <exception>

namespace {

constexpr int kWindowWidth = 960;
constexpr int kWindowHeight = 540;

// Minimal GL 3.3 Core shaders: a fixed triangle with an interpolated vertex color.
constexpr const char* kVertexShaderSource = R"(#version 330 core
layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec3 a_color;
out vec3 v_color;
void main() {
    v_color = a_color;
    gl_Position = vec4(a_pos, 0.0, 1.0);
}
)";

constexpr const char* kFragmentShaderSource = R"(#version 330 core
in vec3 v_color;
out vec4 frag_color;
void main() {
    frag_color = vec4(v_color, 1.0);
}
)";

// Compile a shader stage; report the GL info log and abort on failure.
GLuint compile_shader(GLenum type, const char* source) {
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024] = {};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::fprintf(stderr, "Shader compile failed: %s\n", log);
        std::exit(EXIT_FAILURE);
    }
    return shader;
}

// Link the vertex+fragment program; report the GL info log and abort on failure.
GLuint build_program() {
    const GLuint vertex = compile_shader(GL_VERTEX_SHADER, kVertexShaderSource);
    const GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, kFragmentShaderSource);

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    GLint ok = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024] = {};
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::fprintf(stderr, "Program link failed: %s\n", log);
        std::exit(EXIT_FAILURE);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return program;
}

// Drain the GL error queue. Returns true if no errors were reported.
bool gl_errors_clean(const char* where) {
    bool clean = true;
    for (GLenum err = glGetError(); err != GL_NO_ERROR; err = glGetError()) {
        std::fprintf(stderr, "GL error 0x%04X at %s\n", err, where);
        clean = false;
    }
    return clean;
}

}  // namespace

int main() {
    try {
        platform::Window window(kWindowWidth, kWindowHeight, "CodePhys - Phase 0");

        const platform::GlInfo info = platform::query_gl_info();
        std::printf("OpenGL version : %s\n", info.version.c_str());
        std::printf("Renderer       : %s\n", info.renderer.c_str());
        std::printf("GLSL version   : %s\n", info.glsl_version.c_str());

        // A single triangle in normalized device coordinates: { x, y, r, g, b } per vertex.
        const float vertices[] = {
            0.0f,  0.6f,  1.0f, 0.2f, 0.2f,  // top    (red)
            -0.6f, -0.5f, 0.2f, 1.0f, 0.2f,  // left   (green)
            0.6f,  -0.5f, 0.2f, 0.4f, 1.0f,  // right  (blue)
        };

        GLuint vao = 0;
        GLuint vbo = 0;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        constexpr GLsizei stride = 5 * sizeof(float);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, nullptr);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                              reinterpret_cast<const void*>(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        const GLuint program = build_program();

        bool reported = false;
        window.run([&](int width, int height) {
            glViewport(0, 0, width, height);
            glClearColor(0.08f, 0.10f, 0.14f, 1.0f);  // known clear color
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(program);
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            // Report the smoke-test result exactly once, after the first frame.
            if (!reported) {
                reported = true;
                if (gl_errors_clean("first frame")) {
                    std::printf("Smoke test: triangle drawn, no GL errors.\n");
                }
            }
        });

        glDeleteProgram(program);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::fprintf(stderr, "Fatal: %s\n", e.what());
        return EXIT_FAILURE;
    }
}
