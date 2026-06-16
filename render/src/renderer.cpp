#include <render/renderer.hpp>

#include <glad/glad.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>

#include <physics/math/constants.hpp>
#include <physics/math/mat4.hpp>

#include "font8x8_basic.h"  // vendored public-domain 8x8 font (see header for source)

namespace render {
namespace {

constexpr int kFontCols = 16;                // glyphs per atlas row
constexpr int kFontRows = 8;                 // glyph rows in the atlas
constexpr int kGlyph = 8;                    // glyph cell size in texels
constexpr int kAtlasW = kFontCols * kGlyph;  // 128
constexpr int kAtlasH = kFontRows * kGlyph;  // 64

const char* kShapeVert = R"(#version 330 core
layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec4 a_color;
uniform mat4 u_view_proj;
out vec4 v_color;
void main() {
    v_color = a_color;
    gl_Position = u_view_proj * vec4(a_pos, 0.0, 1.0);
}
)";

const char* kShapeFrag = R"(#version 330 core
in vec4 v_color;
out vec4 frag_color;
void main() { frag_color = v_color; }
)";

const char* kTextVert = R"(#version 330 core
layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec2 a_uv;
layout (location = 2) in vec4 a_color;
uniform mat4 u_screen;
out vec2 v_uv;
out vec4 v_color;
void main() {
    v_uv = a_uv;
    v_color = a_color;
    gl_Position = u_screen * vec4(a_pos, 0.0, 1.0);
}
)";

const char* kTextFrag = R"(#version 330 core
in vec2 v_uv;
in vec4 v_color;
out vec4 frag_color;
uniform sampler2D u_font;
void main() {
    float a = texture(u_font, v_uv).r;
    frag_color = vec4(v_color.rgb, v_color.a * a);
}
)";

unsigned int compile(GLenum type, const char* src) {
    const GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);
    GLint ok = GL_FALSE;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024] = {};
        glGetShaderInfoLog(sh, sizeof(log), nullptr, log);
        throw std::runtime_error(std::string("Renderer shader compile failed: ") + log);
    }
    return sh;
}

unsigned int link_program(const char* vs_src, const char* fs_src) {
    const GLuint vs = compile(GL_VERTEX_SHADER, vs_src);
    const GLuint fs = compile(GL_FRAGMENT_SHADER, fs_src);
    const GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    GLint ok = GL_FALSE;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024] = {};
        glGetProgramInfoLog(prog, sizeof(log), nullptr, log);
        throw std::runtime_error(std::string("Renderer program link failed: ") + log);
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

}  // namespace

Renderer::Renderer() {
    shape_program_ = link_program(kShapeVert, kShapeFrag);
    text_program_ = link_program(kTextVert, kTextFrag);

    // Shape buffer: vec2 position + vec4 color.
    glGenVertexArrays(1, &shape_vao_);
    glGenBuffers(1, &shape_vbo_);
    glBindVertexArray(shape_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, shape_vbo_);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex),
                          reinterpret_cast<const void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Text buffer: vec2 position + vec2 uv + vec4 color.
    glGenVertexArrays(1, &text_vao_);
    glGenBuffers(1, &text_vbo_);
    glBindVertexArray(text_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, text_vbo_);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex),
                          reinterpret_cast<const void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex),
                          reinterpret_cast<const void*>(4 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // Build the R8 font atlas from the vendored 8x8 bitmap.
    unsigned char atlas[kAtlasW * kAtlasH] = {};
    for (int c = 0; c < 128; ++c) {
        const int cell_col = c % kFontCols;
        const int cell_row = c / kFontCols;
        for (int gy = 0; gy < kGlyph; ++gy) {
            const unsigned char bits = font8x8_basic[c][gy];
            for (int gx = 0; gx < kGlyph; ++gx) {
                if ((bits >> gx) & 1u) {  // bit gx (LSB) is the leftmost column
                    const int px = cell_col * kGlyph + gx;
                    const int py = cell_row * kGlyph + gy;
                    atlas[py * kAtlasW + px] = 255;
                }
            }
        }
    }
    glGenTextures(1, &font_texture_);
    glBindTexture(GL_TEXTURE_2D, font_texture_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, kAtlasW, kAtlasH, 0, GL_RED, GL_UNSIGNED_BYTE,
                 atlas);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Renderer::~Renderer() {
    glDeleteTextures(1, &font_texture_);
    glDeleteBuffers(1, &shape_vbo_);
    glDeleteVertexArrays(1, &shape_vao_);
    glDeleteBuffers(1, &text_vbo_);
    glDeleteVertexArrays(1, &text_vao_);
    glDeleteProgram(shape_program_);
    glDeleteProgram(text_program_);
}

double Renderer::world_per_pixel() const {
    return camera_.viewport_height > 0 ? camera_.world_height / camera_.viewport_height : 1.0;
}

void Renderer::clear(Color color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::begin(const Camera2D& camera) {
    camera_ = camera;
    shape_verts_.clear();
    screen_shape_verts_.clear();
    text_verts_.clear();
    glViewport(0, 0, camera_.viewport_width, camera_.viewport_height);
}

void Renderer::add_triangle(Vec2 a, Vec2 b, Vec2 c, Color col) {
    const ShapeVertex va{
        static_cast<float>(a.x), static_cast<float>(a.y), col.r, col.g, col.b, col.a};
    const ShapeVertex vb{
        static_cast<float>(b.x), static_cast<float>(b.y), col.r, col.g, col.b, col.a};
    const ShapeVertex vc{
        static_cast<float>(c.x), static_cast<float>(c.y), col.r, col.g, col.b, col.a};
    shape_verts_.push_back(va);
    shape_verts_.push_back(vb);
    shape_verts_.push_back(vc);
}

void Renderer::add_quad(Vec2 a, Vec2 b, Vec2 c, Vec2 d, Color col) {
    add_triangle(a, b, c, col);
    add_triangle(a, c, d, col);
}

void Renderer::add_screen_triangle(Vec2 a, Vec2 b, Vec2 c, Color col) {
    screen_shape_verts_.push_back(ShapeVertex{static_cast<float>(a.x), static_cast<float>(a.y),
                                              col.r, col.g, col.b, col.a});
    screen_shape_verts_.push_back(ShapeVertex{static_cast<float>(b.x), static_cast<float>(b.y),
                                              col.r, col.g, col.b, col.a});
    screen_shape_verts_.push_back(ShapeVertex{static_cast<float>(c.x), static_cast<float>(c.y),
                                              col.r, col.g, col.b, col.a});
}

void Renderer::rect_screen(Vec2 top_left, Vec2 size, Color color) {
    const Vec2 a = top_left;
    const Vec2 b{top_left.x + size.x, top_left.y};
    const Vec2 c{top_left.x + size.x, top_left.y + size.y};
    const Vec2 d{top_left.x, top_left.y + size.y};
    add_screen_triangle(a, b, c, color);
    add_screen_triangle(a, c, d, color);
}

void Renderer::line_screen(Vec2 a, Vec2 b, Color color, double width_px) {
    const Vec2 dir = b - a;
    const double len = physics::math::length(dir);
    if (len <= 0.0) {
        return;
    }
    const Vec2 unit = dir / len;
    const Vec2 normal{-unit.y, unit.x};
    const Vec2 off = normal * (0.5 * width_px);
    add_screen_triangle(a - off, b - off, b + off, color);
    add_screen_triangle(a - off, b + off, a + off, color);
}

void Renderer::line2D(Vec2 a, Vec2 b, Color color, double width_px) {
    const Vec2 dir = b - a;
    const double len = physics::math::length(dir);
    if (len <= 0.0) {
        return;
    }
    const Vec2 unit = dir / len;
    const Vec2 normal{-unit.y, unit.x};
    const double half = 0.5 * width_px * world_per_pixel();
    const Vec2 off = normal * half;
    add_quad(a - off, b - off, b + off, a + off, color);
}

void Renderer::circle(Vec2 center, double radius, Color color, bool filled) {
    constexpr int kSegments = 48;
    if (filled) {
        Vec2 prev{center.x + radius, center.y};
        for (int i = 1; i <= kSegments; ++i) {
            const double theta = physics::math::two_pi * i / kSegments;
            const Vec2 cur{center.x + radius * std::cos(theta),
                           center.y + radius * std::sin(theta)};
            add_triangle(center, prev, cur, color);
            prev = cur;
        }
    } else {
        Vec2 prev{center.x + radius, center.y};
        for (int i = 1; i <= kSegments; ++i) {
            const double theta = physics::math::two_pi * i / kSegments;
            const Vec2 cur{center.x + radius * std::cos(theta),
                           center.y + radius * std::sin(theta)};
            line2D(prev, cur, color);
            prev = cur;
        }
    }
}

void Renderer::polygon(std::span<const Vec2> points, Color color, bool filled) {
    if (points.size() < 2) {
        return;
    }
    if (filled) {
        for (std::size_t i = 1; i + 1 < points.size(); ++i) {
            add_triangle(points[0], points[i], points[i + 1], color);  // convex fan
        }
    } else {
        for (std::size_t i = 0; i < points.size(); ++i) {
            line2D(points[i], points[(i + 1) % points.size()], color);
        }
    }
}

void Renderer::arrow2D(Vec2 from, Vec2 to, Color color, double width_px) {
    const Vec2 dir = to - from;
    const double len = physics::math::length(dir);
    if (len <= 0.0) {
        return;
    }
    const Vec2 unit = dir / len;
    const Vec2 normal{-unit.y, unit.x};
    const double head_len = (10.0 + 2.0 * width_px) * world_per_pixel();
    const double head_half = (5.0 + width_px) * world_per_pixel();
    const double shaft_len = len > head_len ? len - head_len : 0.0;
    const Vec2 base = from + unit * shaft_len;
    if (shaft_len > 0.0) {
        line2D(from, base, color, width_px);
    }
    add_triangle(to, base + normal * head_half, base - normal * head_half, color);
}

void Renderer::text(Vec2 screen_pos, std::string_view str, Color color, float scale) {
    const float cell = kGlyph * scale;
    float pen_x = static_cast<float>(screen_pos.x);
    float pen_y = static_cast<float>(screen_pos.y);
    for (const char ch : str) {
        if (ch == '\n') {
            pen_x = static_cast<float>(screen_pos.x);
            pen_y += cell;
            continue;
        }
        const unsigned char c = static_cast<unsigned char>(ch);
        if (c < 128) {
            const int cell_col = c % kFontCols;
            const int cell_row = c / kFontCols;
            const float u0 = static_cast<float>(cell_col * kGlyph) / kAtlasW;
            const float v0 = static_cast<float>(cell_row * kGlyph) / kAtlasH;
            const float u1 = static_cast<float>(cell_col * kGlyph + kGlyph) / kAtlasW;
            const float v1 = static_cast<float>(cell_row * kGlyph + kGlyph) / kAtlasH;
            const float x0 = pen_x, y0 = pen_y, x1 = pen_x + cell, y1 = pen_y + cell;
            const auto push = [&](float x, float y, float u, float v) {
                text_verts_.push_back(
                    TextVertex{x, y, u, v, color.r, color.g, color.b, color.a});
            };
            push(x0, y0, u0, v0);
            push(x1, y0, u1, v0);
            push(x1, y1, u1, v1);
            push(x0, y0, u0, v0);
            push(x1, y1, u1, v1);
            push(x0, y1, u0, v1);
        }
        pen_x += cell;
    }
}

Vec2 Renderer::measure_text(std::string_view str, float scale) const {
    const float cell = kGlyph * scale;
    float width = 0.0f;
    float cur = 0.0f;
    int lines = 1;
    for (const char ch : str) {
        if (ch == '\n') {
            width = std::max(width, cur);
            cur = 0.0f;
            ++lines;
        } else {
            cur += cell;
        }
    }
    width = std::max(width, cur);
    return Vec2{width, static_cast<double>(lines) * cell};
}

float Renderer::text_line_height(float scale) const {
    return kGlyph * scale;
}

int Renderer::viewport_width() const {
    return camera_.viewport_width;
}

int Renderer::viewport_height() const {
    return camera_.viewport_height;
}

void Renderer::end() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // World-space shapes.
    if (!shape_verts_.empty()) {
        const auto vp = physics::math::to_float_array(camera_.view_projection());
        glUseProgram(shape_program_);
        glUniformMatrix4fv(glGetUniformLocation(shape_program_, "u_view_proj"), 1, GL_FALSE,
                           vp.data());
        glBindVertexArray(shape_vao_);
        glBindBuffer(GL_ARRAY_BUFFER, shape_vbo_);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(shape_verts_.size() * sizeof(ShapeVertex)),
                     shape_verts_.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(shape_verts_.size()));
    }

    const auto screen = physics::math::to_float_array(
        physics::math::ortho(0.0, static_cast<double>(camera_.viewport_width),
                             static_cast<double>(camera_.viewport_height), 0.0, -1.0, 1.0));

    // Screen-space shapes (UI), drawn over world geometry, under text.
    if (!screen_shape_verts_.empty()) {
        glUseProgram(shape_program_);
        glUniformMatrix4fv(glGetUniformLocation(shape_program_, "u_view_proj"), 1, GL_FALSE,
                           screen.data());
        glBindVertexArray(shape_vao_);
        glBindBuffer(GL_ARRAY_BUFFER, shape_vbo_);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(screen_shape_verts_.size() * sizeof(ShapeVertex)),
                     screen_shape_verts_.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(screen_shape_verts_.size()));
    }

    // Screen-space text (origin top-left).
    if (!text_verts_.empty()) {
        glUseProgram(text_program_);
        glUniformMatrix4fv(glGetUniformLocation(text_program_, "u_screen"), 1, GL_FALSE,
                           screen.data());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, font_texture_);
        glUniform1i(glGetUniformLocation(text_program_, "u_font"), 0);
        glBindVertexArray(text_vao_);
        glBindBuffer(GL_ARRAY_BUFFER, text_vbo_);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(text_verts_.size() * sizeof(TextVertex)),
                     text_verts_.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(text_verts_.size()));
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

}  // namespace render
