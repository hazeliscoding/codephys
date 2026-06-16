#pragma once

#include <span>
#include <string_view>
#include <vector>

#include <physics/math/vec2.hpp>
#include <render/camera2d.hpp>
#include <render/color.hpp>

// Renderer-agnostic 2D drawing API over OpenGL 3.3 (DESIGN §7.2). All GL state lives in
// the implementation; this header exposes no GL types (handles are plain `unsigned`), so
// scenes can draw without including GLFW/OpenGL. Requires a current GL context for the
// lifetime of the Renderer. Depends on physics::math.

namespace render {

using physics::math::Vec2;

class Renderer {
public:
    Renderer();  // creates GL programs/buffers/font; needs a current GL context
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    // Clear the framebuffer to a solid color.
    void clear(Color color);

    // Begin a frame: set the camera view-projection, store the viewport, clear batches.
    void begin(const Camera2D& camera);
    // Flush batched world geometry, then screen-space text.
    void end();

    // World-space primitives.
    void line2D(Vec2 a, Vec2 b, Color color, double width_px = 1.5);
    void circle(Vec2 center, double radius, Color color, bool filled = true);
    void polygon(std::span<const Vec2> points, Color color, bool filled = true);
    void arrow2D(Vec2 from, Vec2 to, Color color, double width_px = 2.0);

    // Screen-space primitives (origin top-left, pixels) — for the UI, drawn over the
    // world geometry but under text.
    void rect_screen(Vec2 top_left, Vec2 size, Color color);
    void line_screen(Vec2 a, Vec2 b, Color color, double width_px = 1.0);

    // Screen-space text (origin top-left, pixels). `scale` multiplies the 8x8 glyph cell.
    void text(Vec2 screen_pos, std::string_view str, Color color, float scale = 2.0f);
    Vec2 measure_text(std::string_view str, float scale = 2.0f) const;
    float text_line_height(float scale = 2.0f) const;

    // Current frame's viewport size in pixels (valid between begin/end).
    int viewport_width() const;
    int viewport_height() const;

private:
    struct ShapeVertex {
        float x, y;
        float r, g, b, a;
    };
    struct TextVertex {
        float x, y;
        float u, v;
        float r, g, b, a;
    };

    void add_triangle(Vec2 a, Vec2 b, Vec2 c, Color color);
    void add_quad(Vec2 a, Vec2 b, Vec2 c, Vec2 d, Color color);
    void add_screen_triangle(Vec2 a, Vec2 b, Vec2 c, Color color);
    double world_per_pixel() const;

    unsigned int shape_program_ = 0;
    unsigned int shape_vao_ = 0;
    unsigned int shape_vbo_ = 0;
    unsigned int text_program_ = 0;
    unsigned int text_vao_ = 0;
    unsigned int text_vbo_ = 0;
    unsigned int font_texture_ = 0;

    std::vector<ShapeVertex> shape_verts_;
    std::vector<ShapeVertex> screen_shape_verts_;
    std::vector<TextVertex> text_verts_;
    Camera2D camera_;
};

}  // namespace render
