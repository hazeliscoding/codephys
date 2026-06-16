#include <physics/core/integrator.hpp>

// Integrator implementations (DESIGN §8.2). Citations: College Physics 2e §2 (kinematics)
// for Euler-family stepping. Primary sources for each scheme are noted below and listed in
// docs/REFERENCES.md / REFERENCES.bib.

namespace physics::core {

namespace {

// Explicit (Forward) Euler — simplest; visibly gains energy. Teaching baseline.
// Source: Euler (1768), Institutionum calculi integralis. O(dt) local error.
State step_explicit_euler(const State& s, const AccelFn& accel, double t, double dt) {
    const Vec2 a = accel(s.position, s.velocity, t);
    return State{s.position + s.velocity * dt, s.velocity + a * dt};
}

// Semi-implicit (symplectic) Euler — update velocity first, then position with the new
// velocity. Being symplectic, it has BOUNDED energy drift for oscillators/orbits (not exact
// conservation). Source: Störmer (1907); Hairer, Lubich & Wanner (2003, 2006).
State step_semi_implicit_euler(const State& s, const AccelFn& accel, double t, double dt) {
    const Vec2 a = accel(s.position, s.velocity, t);
    const Vec2 v_new = s.velocity + a * dt;
    return State{s.position + v_new * dt, v_new};
}

// Velocity Verlet — time-reversible, symplectic, O(dt^3) local error; assumes acceleration
// independent of velocity (true for Phase 1 force models). College Physics 2e §2.5
// (constant-accel kinematics). Source: Verlet (1967); velocity form, Swope et al. (1982).
State step_verlet(const State& s, const AccelFn& accel, double t, double dt) {
    const Vec2 a = accel(s.position, s.velocity, t);
    const Vec2 p_new = s.position + s.velocity * dt + a * (0.5 * dt * dt);
    const Vec2 a_new = accel(p_new, s.velocity, t + dt);
    const Vec2 v_new = s.velocity + (a + a_new) * (0.5 * dt);
    return State{p_new, v_new};
}

// Classic 4th-order Runge-Kutta on y = (position, velocity). Gold standard for accuracy,
// O(dt^5) local error. Source: Runge (1895); Kutta (1901).
State step_rk4(const State& s, const AccelFn& accel, double t, double dt) {
    const Vec2 dp1 = s.velocity;
    const Vec2 dv1 = accel(s.position, s.velocity, t);

    const Vec2 dp2 = s.velocity + dv1 * (dt * 0.5);
    const Vec2 dv2 =
        accel(s.position + dp1 * (dt * 0.5), s.velocity + dv1 * (dt * 0.5), t + dt * 0.5);

    const Vec2 dp3 = s.velocity + dv2 * (dt * 0.5);
    const Vec2 dv3 =
        accel(s.position + dp2 * (dt * 0.5), s.velocity + dv2 * (dt * 0.5), t + dt * 0.5);

    const Vec2 dp4 = s.velocity + dv3 * dt;
    const Vec2 dv4 = accel(s.position + dp3 * dt, s.velocity + dv3 * dt, t + dt);

    const Vec2 position = s.position + (dp1 + dp2 * 2.0 + dp3 * 2.0 + dp4) * (dt / 6.0);
    const Vec2 velocity = s.velocity + (dv1 + dv2 * 2.0 + dv3 * 2.0 + dv4) * (dt / 6.0);
    return State{position, velocity};
}

}  // namespace

State integrate(Integrator scheme, const State& s, const AccelFn& accel, double t, double dt) {
    switch (scheme) {
        case Integrator::ExplicitEuler:
            return step_explicit_euler(s, accel, t, dt);
        case Integrator::SemiImplicitEuler:
            return step_semi_implicit_euler(s, accel, t, dt);
        case Integrator::Verlet:
            return step_verlet(s, accel, t, dt);
        case Integrator::RK4:
            return step_rk4(s, accel, t, dt);
    }
    return s;  // unreachable; keeps the compiler happy
}

}  // namespace physics::core
