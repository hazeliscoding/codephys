#pragma once

// Umbrella header for the render module (DESIGN §7.2). Phase 1 = 2D renderer + camera +
// color. render depends downward only on physics::math (and OpenGL internally).

#include <render/camera2d.hpp>
#include <render/color.hpp>
#include <render/renderer.hpp>
