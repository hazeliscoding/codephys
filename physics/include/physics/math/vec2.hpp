#pragma once

// Phase 0 placeholder for the physics math module (DESIGN §8.1). Real vec/mat/quaternion
// types and units arrive in Phase 1; this exists so the `physics` target compiles and the
// headless test harness has something to assert against.
//
// HARD RULE (CLAUDE.md): physics/ depends on the C++ standard library ONLY. No GLFW,
// OpenGL, glad, render, or ui includes may ever appear under physics/.

namespace physics::math {

// A minimal 2D vector. Units are a caller concern (SI internally); this is pure math.
struct Vec2 {
    double x = 0.0;
    double y = 0.0;
};

constexpr Vec2 operator+(Vec2 a, Vec2 b) {
    return {a.x + b.x, a.y + b.y};
}

// Midpoint of two points. Trivial, but gives the Phase 0 test a real assertion.
Vec2 midpoint(Vec2 a, Vec2 b);

}  // namespace physics::math
