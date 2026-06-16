#pragma once

#include <array>

#include <physics/math/vec2.hpp>

// Per-frame input snapshot consumed by the immediate-mode UI and scenes. The application
// builds this from platform/GLFW each frame and passes it in, so `ui` depends only on the
// renderer and math — never on GLFW/platform (DESIGN §D6 layering).

namespace ui {

using physics::math::Vec2;

// Key indices use the platform's key codes (GLFW numbering); kKeyCount covers GLFW_KEY_LAST.
inline constexpr int kKeyCount = 349;

struct InputState {
    Vec2 mouse{};                 // screen pixels, origin top-left
    bool mouse_down = false;      // left button currently held
    bool mouse_pressed = false;   // left button went down this frame
    bool mouse_released = false;  // left button went up this frame
    double scroll = 0.0;          // wheel delta this frame

    std::array<bool, kKeyCount> key_down{};     // currently held, indexed by key code
    std::array<bool, kKeyCount> key_pressed{};  // went down this frame
};

}  // namespace ui
