#pragma once

#include <span>
#include <string_view>

#include <physics/math/vec2.hpp>
#include <render/renderer.hpp>
#include <ui/input.hpp>

// Immediate-mode UI (DESIGN §7.3). Widgets are re-declared each frame and draw through the
// Renderer's screen-space primitives. A single current panel context is set by begin(); the
// widget free functions operate on it. ui depends downward on render only.

namespace ui {

using physics::math::Vec2;

// Begin/end a vertically-stacked panel. Widgets called between these operate on it.
// `width` is the panel width in pixels; a translucent background is drawn behind it.
void begin(render::Renderer& renderer, const InputState& input, Vec2 origin = {12.0, 12.0},
           double width = 300.0);
void end();

// Widgets. Slider/checkbox edit caller-owned state; button returns true on the activating
// frame. All read the current panel's input snapshot.
bool sliderFloat(std::string_view label, double* value, double min_value, double max_value);
bool button(std::string_view label);
bool checkbox(std::string_view label, bool* value);
void label(std::string_view text);
void labelEquation(std::string_view equation);
void plotLine(std::string_view label, std::span<const double> values, float height_px = 60.0f);

// Structure / onboarding helpers.
void heading(std::string_view text);  // larger, bright title text
void help(std::string_view text);     // dim, word-wrapped explanatory text
void separator();                     // thin divider line

}  // namespace ui
