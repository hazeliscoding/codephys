#pragma once

#include <functional>
#include <vector>

#include <physics/core/integrator.hpp>
#include <physics/core/particle.hpp>
#include <physics/math/vec2.hpp>

// The simulation World (DESIGN §8.3): owns particles, runs the step pipeline, and reports
// conserved quantities. Phase 1 has no ForceGenerator system yet — acceleration comes from
// a per-world AccelFn — but the pipeline is shaped so Phase 2 forces drop in without
// changing step(). stdlib + physics::math only.

namespace physics::core {

using physics::math::Vec2;

// Conserved/derived quantities recorded each tick (DESIGN §8.7), for readouts and tests.
struct Conserved {
    double kinetic_energy = 0.0;    // J
    double potential_energy = 0.0;  // J
    double total_energy = 0.0;      // J
    Vec2 momentum{};                // kg·m/s
    double angular_momentum = 0.0;  // kg·m^2/s about the origin (2D scalar)
};

// Per-particle potential energy [J] for reporting; depends on the scene's force model.
using PotentialEnergyFn = std::function<double(const Particle&)>;

class World {
public:
    void add(const Particle& p);
    void clear();

    // The acceleration law applied to each particle: a(position, velocity, t).
    void set_acceleration(AccelFn accel);
    // Optional potential-energy model for reporting (default: zero).
    void set_potential_energy(PotentialEnergyFn potential);
    void set_integrator(Integrator scheme);
    Integrator integrator() const { return integrator_; }

    // Advance one tick: clear accumulators -> compute acceleration -> integrate (selected
    // scheme) -> record conserved quantities.
    void step(double dt);

    double time() const { return time_; }
    const std::vector<Particle>& particles() const { return particles_; }
    std::vector<Particle>& particles() { return particles_; }
    const Conserved& conserved() const { return conserved_; }

private:
    void record_conserved();

    std::vector<Particle> particles_;
    AccelFn acceleration_;
    PotentialEnergyFn potential_energy_;
    Integrator integrator_ = Integrator::SemiImplicitEuler;
    double time_ = 0.0;
    Conserved conserved_{};
};

}  // namespace physics::core
