#include <ui/ui.hpp>

#include <algorithm>
#include <cstdio>
#include <string>

#include <render/color.hpp>

// Immediate-mode UI implementation (DESIGN §7.3). A single file-static context holds the
// current panel; `s_active_item` persists across frames so sliders keep tracking the mouse
// during a drag. Layout math is in double to match the renderer's Vec2 (px) coordinates.

namespace ui {
namespace {

using render::Color;

struct Context {
    render::Renderer* renderer = nullptr;
    InputState input{};
    double origin_x = 0.0;
    double cursor_y = 0.0;
    double width = 360.0;
    double pad = 8.0;
    float scale = 2.5f;  // base body text scale (8x8 glyph -> 20 px)
    double line_h = 20.0;
    int item_counter = 0;
};

Context g_ctx;
int s_active_item = -1;  // widget being dragged; persists across frames

constexpr Color kPanelBg{0.04f, 0.05f, 0.08f, 0.86f};  // translucent backdrop for contrast
constexpr Color kHeading{1.00f, 1.00f, 1.00f, 1.0f};
constexpr Color kHelpText{0.66f, 0.71f, 0.80f, 1.0f};
constexpr Color kSeparatorCol{0.30f, 0.34f, 0.42f, 1.0f};
constexpr Color kPanelText{0.92f, 0.94f, 0.97f, 1.0f};
constexpr Color kEqnText{0.95f, 0.85f, 0.30f, 1.0f};
constexpr Color kTrack{0.22f, 0.24f, 0.30f, 1.0f};
constexpr Color kFill{0.30f, 0.55f, 0.95f, 1.0f};
constexpr Color kButton{0.26f, 0.30f, 0.38f, 1.0f};
constexpr Color kButtonHot{0.34f, 0.40f, 0.52f, 1.0f};
constexpr Color kBox{0.18f, 0.20f, 0.26f, 1.0f};

bool point_in(Vec2 p, Vec2 tl, Vec2 size) {
    return p.x >= tl.x && p.x <= tl.x + size.x && p.y >= tl.y && p.y <= tl.y + size.y;
}

std::string format_value(double v) {
    char buf[32] = {};
    std::snprintf(buf, sizeof(buf), "%.3g", v);
    return std::string(buf);
}

}  // namespace

void begin(render::Renderer& renderer, const InputState& input, Vec2 origin, double width) {
    g_ctx.renderer = &renderer;
    g_ctx.input = input;
    g_ctx.origin_x = origin.x + g_ctx.pad;  // inner left padding
    g_ctx.cursor_y = origin.y + g_ctx.pad;  // inner top padding
    g_ctx.width = width - 2.0 * g_ctx.pad;  // content width (keeps text off both edges)
    g_ctx.line_h = renderer.text_line_height(g_ctx.scale);
    g_ctx.item_counter = 0;
    if (!input.mouse_down) {
        s_active_item = -1;  // release ends any drag
    }

    // Translucent full-height backdrop so panel text stays legible over the scene. Drawn
    // first (in begin) so it sits behind the widgets.
    renderer.rect_screen(Vec2{origin.x, 0.0},
                         Vec2{width, static_cast<double>(renderer.viewport_height())},
                         kPanelBg);
}

void end() {
    g_ctx.renderer = nullptr;
}

void label(std::string_view text) {
    g_ctx.renderer->text(Vec2{g_ctx.origin_x, g_ctx.cursor_y}, text, kPanelText, g_ctx.scale);
    g_ctx.cursor_y += g_ctx.line_h + g_ctx.pad;
}

void labelEquation(std::string_view equation) {
    g_ctx.renderer->text(Vec2{g_ctx.origin_x, g_ctx.cursor_y}, equation, kEqnText,
                         g_ctx.scale);
    g_ctx.cursor_y += g_ctx.line_h + g_ctx.pad;
}

void heading(std::string_view text) {
    const float hs = g_ctx.scale * 1.3f;
    g_ctx.renderer->text(Vec2{g_ctx.origin_x, g_ctx.cursor_y}, text, kHeading, hs);
    g_ctx.cursor_y += g_ctx.renderer->text_line_height(hs) + g_ctx.pad;
}

void help(std::string_view text) {
    const double glyph = 8.0 * g_ctx.scale;
    const int max_chars = std::max(1, static_cast<int>(g_ctx.width / glyph));
    std::string line;
    auto flush = [&] {
        if (!line.empty()) {
            g_ctx.renderer->text(Vec2{g_ctx.origin_x, g_ctx.cursor_y}, line, kHelpText,
                                 g_ctx.scale);
            g_ctx.cursor_y += g_ctx.line_h + 2.0;
            line.clear();
        }
    };
    std::size_t i = 0;
    while (i < text.size()) {
        const std::size_t next = text.find(' ', i);
        const std::string_view word =
            text.substr(i, (next == std::string_view::npos ? text.size() : next) - i);
        if (line.empty()) {
            line = std::string(word);
        } else if (line.size() + 1 + word.size() <= static_cast<std::size_t>(max_chars)) {
            line += ' ';
            line += word;
        } else {
            flush();
            line = std::string(word);
        }
        if (next == std::string_view::npos) {
            break;
        }
        i = next + 1;
    }
    flush();
    g_ctx.cursor_y += g_ctx.pad * 0.5;
}

void separator() {
    g_ctx.cursor_y += g_ctx.pad * 0.5;
    g_ctx.renderer->line_screen(Vec2{g_ctx.origin_x, g_ctx.cursor_y},
                                Vec2{g_ctx.origin_x + g_ctx.width, g_ctx.cursor_y},
                                kSeparatorCol, 1.0);
    g_ctx.cursor_y += g_ctx.pad;
}

bool button(std::string_view text) {
    const Vec2 tl{g_ctx.origin_x, g_ctx.cursor_y};
    const Vec2 size{g_ctx.width, g_ctx.line_h + 2.0 * g_ctx.pad};
    const bool hovered = point_in(g_ctx.input.mouse, tl, size);
    g_ctx.renderer->rect_screen(tl, size, hovered ? kButtonHot : kButton);
    g_ctx.renderer->text(Vec2{tl.x + g_ctx.pad, tl.y + g_ctx.pad}, text, kPanelText,
                         g_ctx.scale);
    g_ctx.cursor_y += size.y + g_ctx.pad;
    return hovered && g_ctx.input.mouse_pressed;
}

bool checkbox(std::string_view text, bool* value) {
    const double box = g_ctx.line_h;
    const Vec2 tl{g_ctx.origin_x, g_ctx.cursor_y};
    const Vec2 row_size{g_ctx.width, box};
    const bool hovered = point_in(g_ctx.input.mouse, tl, row_size);
    bool changed = false;
    if (hovered && g_ctx.input.mouse_pressed) {
        *value = !*value;
        changed = true;
    }
    g_ctx.renderer->rect_screen(tl, Vec2{box, box}, kBox);
    if (*value) {
        const double inset = box * 0.25;
        g_ctx.renderer->rect_screen(Vec2{tl.x + inset, tl.y + inset},
                                    Vec2{box - 2.0 * inset, box - 2.0 * inset}, kFill);
    }
    g_ctx.renderer->text(Vec2{tl.x + box + g_ctx.pad, tl.y}, text, kPanelText, g_ctx.scale);
    g_ctx.cursor_y += box + g_ctx.pad;
    return changed;
}

bool sliderFloat(std::string_view text, double* value, double min_value, double max_value) {
    const int id = g_ctx.item_counter++;
    const std::string caption = std::string(text) + ": " + format_value(*value);
    g_ctx.renderer->text(Vec2{g_ctx.origin_x, g_ctx.cursor_y}, caption, kPanelText,
                         g_ctx.scale);
    g_ctx.cursor_y += g_ctx.line_h + 2.0;

    const Vec2 track_tl{g_ctx.origin_x, g_ctx.cursor_y};
    const Vec2 track_size{g_ctx.width, g_ctx.line_h};
    const bool hovered = point_in(g_ctx.input.mouse, track_tl, track_size);
    if (hovered && g_ctx.input.mouse_pressed) {
        s_active_item = id;
    }

    const double span = max_value - min_value;
    bool changed = false;
    if (s_active_item == id && g_ctx.input.mouse_down && span != 0.0) {
        double t = (g_ctx.input.mouse.x - track_tl.x) / track_size.x;
        t = std::clamp(t, 0.0, 1.0);
        const double new_value = min_value + t * span;
        if (new_value != *value) {
            *value = new_value;
            changed = true;
        }
    }

    const double frac = span != 0.0 ? std::clamp((*value - min_value) / span, 0.0, 1.0) : 0.0;
    g_ctx.renderer->rect_screen(track_tl, track_size, kTrack);
    g_ctx.renderer->rect_screen(track_tl, Vec2{track_size.x * frac, track_size.y}, kFill);
    const double knob = g_ctx.line_h * 0.5;
    g_ctx.renderer->rect_screen(
        Vec2{track_tl.x + track_size.x * frac - knob * 0.5, track_tl.y - 1.0},
        Vec2{knob, track_size.y + 2.0}, render::colors::white);
    g_ctx.cursor_y += track_size.y + g_ctx.pad;
    return changed;
}

void plotLine(std::string_view text, std::span<const double> values, float height_px) {
    g_ctx.renderer->text(Vec2{g_ctx.origin_x, g_ctx.cursor_y}, text, kPanelText, g_ctx.scale);
    g_ctx.cursor_y += g_ctx.line_h + 2.0;

    const Vec2 tl{g_ctx.origin_x, g_ctx.cursor_y};
    const Vec2 size{g_ctx.width, static_cast<double>(height_px)};
    g_ctx.renderer->rect_screen(tl, size, kBox);

    if (values.size() >= 2) {
        double lo = values[0];
        double hi = values[0];
        for (const double v : values) {
            lo = std::min(lo, v);
            hi = std::max(hi, v);
        }
        const double range = (hi - lo) != 0.0 ? (hi - lo) : 1.0;
        const double inset = 3.0;
        const double w = size.x - 2.0 * inset;
        const double h = size.y - 2.0 * inset;
        auto point_at = [&](std::size_t i) {
            const double fx = static_cast<double>(i) / static_cast<double>(values.size() - 1);
            const double fy = (values[i] - lo) / range;
            return Vec2{tl.x + inset + fx * w, tl.y + inset + (1.0 - fy) * h};
        };
        for (std::size_t i = 1; i < values.size(); ++i) {
            g_ctx.renderer->line_screen(point_at(i - 1), point_at(i), kFill, 1.5);
        }
    }
    g_ctx.cursor_y += size.y + g_ctx.pad;
}

}  // namespace ui
