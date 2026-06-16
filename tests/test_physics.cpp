#include <cmath>

#include <doctest/doctest.h>

#include <physics/core/integrator.hpp>
#include <physics/core/world.hpp>
#include <physics/math/constants.hpp>
#include <physics/math/units.hpp>

using namespace physics::core;
using physics::math::g_earth;
using physics::math::pi;
using physics::math::Vec2;

namespace {

// Integrate a simple harmonic oscillator x'' = -omega^2 x to time T and return the error
// in x against the analytic solution A*cos(omega*t). Used for convergence-order tests.
double shm_position_error(Integrator scheme, double dt) {
    const double omega = 2.0;
    const double amplitude = 1.0;
    const double target_time = 1.0;
    const AccelFn accel = [omega](Vec2 p, Vec2, double) {
        return Vec2{-omega * omega * p.x, -omega * omega * p.y};
    };
    State s{Vec2{amplitude, 0.0}, Vec2{0.0, 0.0}};
    const int steps = static_cast<int>(std::lround(target_time / dt));
    double t = 0.0;
    for (int i = 0; i < steps; ++i) {
        s = integrate(scheme, s, accel, t, dt);
        t += dt;
    }
    const double exact = amplitude * std::cos(omega * t);
    return std::abs(s.position.x - exact);
}

}  // namespace

// College Physics 2e §3.4 — projectile range, max height, time of flight.
TEST_CASE("projectile matches closed-form solution") {
    const double v0 = 30.0;  // m/s
    const double angle = physics::math::deg_to_rad(40.0);
    const double g = g_earth;
    const AccelFn gravity = [g](Vec2, Vec2, double) { return Vec2{0.0, -g}; };

    State s{Vec2{0.0, 0.0}, Vec2{v0 * std::cos(angle), v0 * std::sin(angle)}};
    const double flight = 2.0 * v0 * std::sin(angle) / g;
    const double dt = 1.0e-4;
    const int steps = static_cast<int>(std::lround(flight / dt));

    double max_height = 0.0;
    double t = 0.0;
    for (int i = 0; i < steps; ++i) {
        s = integrate(Integrator::RK4, s, gravity, t, dt);
        t += dt;
        max_height = std::max(max_height, s.position.y);
    }

    const double range = v0 * v0 * std::sin(2.0 * angle) / g;
    const double height = (v0 * std::sin(angle)) * (v0 * std::sin(angle)) / (2.0 * g);
    CHECK(s.position.x == doctest::Approx(range).epsilon(1e-3));
    CHECK(s.position.y == doctest::Approx(0.0).epsilon(1e-3));
    CHECK(max_height == doctest::Approx(height).epsilon(1e-3));
}

// College Physics 2e §6.6 — circular orbit period vs Kepler's third law.
TEST_CASE("circular orbit returns after one Kepler period") {
    const double mu = 1.0;  // GM
    const double radius = 1.0;
    const double speed = std::sqrt(mu / radius);  // circular-orbit speed
    const AccelFn central = [mu](Vec2 p, Vec2, double) {
        const double r = physics::math::length(p);
        return p * (-mu / (r * r * r));
    };
    State s{Vec2{radius, 0.0}, Vec2{0.0, speed}};
    const double period = 2.0 * pi * std::sqrt(radius * radius * radius / mu);  // Kepler
    const double dt = period / 20000.0;
    const int steps = 20000;
    const double l0 = physics::math::cross(s.position, s.velocity);  // angular momentum / m
    double t = 0.0;
    for (int i = 0; i < steps; ++i) {
        s = integrate(Integrator::RK4, s, central, t, dt);
        t += dt;
    }
    // After one period the body returns to its start; angular momentum is conserved.
    CHECK(s.position.x == doctest::Approx(radius).epsilon(1e-3));
    CHECK(s.position.y == doctest::Approx(0.0).epsilon(1e-2));
    CHECK(physics::math::cross(s.position, s.velocity) == doctest::Approx(l0).epsilon(1e-6));
}

TEST_CASE("integrators converge at their expected order") {
    SUBCASE("Explicit Euler is first order") {
        const double e1 = shm_position_error(Integrator::ExplicitEuler, 1.0e-3);
        const double e2 = shm_position_error(Integrator::ExplicitEuler, 0.5e-3);
        CHECK(e1 / e2 == doctest::Approx(2.0).epsilon(0.2));  // halving dt halves error
    }
    SUBCASE("RK4 is fourth order") {
        const double e1 = shm_position_error(Integrator::RK4, 0.05);
        const double e2 = shm_position_error(Integrator::RK4, 0.025);
        CHECK(e1 / e2 > 10.0);  // ~16x; well above the first/second-order schemes
    }
}

TEST_CASE("symplectic integrators keep energy bounded over many periods") {
    const double omega = 2.0;
    const AccelFn accel = [omega](Vec2 p, Vec2, double) {
        return Vec2{-omega * omega * p.x, 0.0};
    };
    auto final_energy = [&](Integrator scheme) {
        State s{Vec2{1.0, 0.0}, Vec2{0.0, 0.0}};
        const double dt = 0.01;
        const int steps = static_cast<int>(std::lround(50.0 * (2.0 * pi / omega) / dt));
        double t = 0.0;
        for (int i = 0; i < steps; ++i) {
            s = integrate(scheme, s, accel, t, dt);
            t += dt;
        }
        return 0.5 * s.velocity.x * s.velocity.x +
               0.5 * omega * omega * s.position.x * s.position.x;
    };
    const double e0 = 0.5 * omega * omega;  // initial energy (x=1, v=0)
    CHECK(final_energy(Integrator::SemiImplicitEuler) == doctest::Approx(e0).epsilon(0.05));
    CHECK(final_energy(Integrator::Verlet) == doctest::Approx(e0).epsilon(0.01));
    CHECK(final_energy(Integrator::ExplicitEuler) > e0 * 1.5);  // visibly gains energy
}

TEST_CASE("World conserves momentum with no external force") {
    World w;
    w.set_integrator(Integrator::Verlet);
    w.add(Particle{Vec2{0.0, 0.0}, Vec2{2.0, 1.0}, 3.0, Vec2{}});
    w.add(Particle{Vec2{5.0, 0.0}, Vec2{-1.0, 4.0}, 2.0, Vec2{}});
    const Vec2 p0 = w.conserved().momentum;  // recorded lazily; compute initial directly
    const Vec2 expected{3.0 * 2.0 + 2.0 * -1.0, 3.0 * 1.0 + 2.0 * 4.0};
    for (int i = 0; i < 100; ++i) {
        w.step(0.01);
    }
    CHECK(w.conserved().momentum.x == doctest::Approx(expected.x));
    CHECK(w.conserved().momentum.y == doctest::Approx(expected.y));
    (void)p0;
}

TEST_CASE("simulation is deterministic") {
    auto run = [] {
        World w;
        w.set_integrator(Integrator::RK4);
        w.set_acceleration([](Vec2, Vec2, double) { return Vec2{0.0, -9.81}; });
        w.add(Particle{Vec2{0.0, 0.0}, Vec2{10.0, 20.0}, 1.0, Vec2{}});
        for (int i = 0; i < 500; ++i) {
            w.step(0.001);
        }
        return w.particles().front();
    };
    const Particle a = run();
    const Particle b = run();
    CHECK(a.position == b.position);  // bitwise-identical trajectories
    CHECK(a.velocity == b.velocity);
}
