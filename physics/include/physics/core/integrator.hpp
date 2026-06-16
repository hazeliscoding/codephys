#pragma once

#include <functional>

#include <physics/math/vec2.hpp>

// Switchable ODE integrators (DESIGN §8.2) — the pedagogical centerpiece. Motion is
// dy/dt = f(y, t) with y = (position, velocity). Each scheme advances one (position,
// velocity) state given an acceleration function a(position, velocity, t). Keeping the
// integrators decoupled from World makes them small and independently testable.

namespace physics::core {

using physics::math::Vec2;

enum class Integrator { ExplicitEuler, SemiImplicitEuler, Verlet, RK4 };

// Acceleration as a function of state: a(position, velocity, t) -> m/s^2.
// Phase 1 force models (uniform gravity, central inverse-square) are velocity-independent.
using AccelFn = std::function<Vec2(Vec2 position, Vec2 velocity, double t)>;

struct State {
    Vec2 position;
    Vec2 velocity;
};

// Advance `s` by one timestep `dt` (starting at time `t`) under `scheme`.
State integrate(Integrator scheme, const State& s, const AccelFn& accel, double t, double dt);

}  // namespace physics::core
