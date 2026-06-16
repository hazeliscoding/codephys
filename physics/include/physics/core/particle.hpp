#pragma once

#include <physics/math/vec2.hpp>

// A point mass (DESIGN §8.3). All quantities SI: position [m], velocity [m/s], mass [kg],
// force accumulator [N]. Phase 1 is 2D. stdlib + physics::math only.

namespace physics::core {

using physics::math::Vec2;

struct Particle {
    Vec2 position;       // m
    Vec2 velocity;       // m/s
    double mass = 1.0;   // kg
    Vec2 force_accum{};  // N, cleared at the start of each tick (DESIGN §8.3 pipeline)
};

}  // namespace physics::core
