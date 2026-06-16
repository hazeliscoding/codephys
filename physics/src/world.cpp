#include <physics/core/world.hpp>

#include <utility>

#include <physics/math/vec2.hpp>

namespace physics::core {

using physics::math::cross;
using physics::math::dot;

void World::add(const Particle& p) {
    particles_.push_back(p);
}

void World::clear() {
    particles_.clear();
    time_ = 0.0;
    conserved_ = Conserved{};
}

void World::set_acceleration(AccelFn accel) {
    acceleration_ = std::move(accel);
}

void World::set_potential_energy(PotentialEnergyFn potential) {
    potential_energy_ = std::move(potential);
}

void World::set_integrator(Integrator scheme) {
    integrator_ = scheme;
}

void World::step(double dt) {
    static const AccelFn zero_accel = [](Vec2, Vec2, double) { return Vec2{}; };
    const AccelFn& accel = acceleration_ ? acceleration_ : zero_accel;

    for (Particle& p : particles_) {
        p.force_accum = Vec2{};  // clear accumulators (DESIGN §8.3); unused until Phase 2
        const State next =
            integrate(integrator_, State{p.position, p.velocity}, accel, time_, dt);
        p.position = next.position;
        p.velocity = next.velocity;
    }

    time_ += dt;
    record_conserved();
}

void World::record_conserved() {
    Conserved c{};
    for (const Particle& p : particles_) {
        c.kinetic_energy += 0.5 * p.mass * dot(p.velocity, p.velocity);
        if (potential_energy_) {
            c.potential_energy += potential_energy_(p);
        }
        c.momentum += p.velocity * p.mass;
        // 2D angular momentum about the origin: m * (r x v) z-component.
        c.angular_momentum += p.mass * cross(p.position, p.velocity);
    }
    c.total_energy = c.kinetic_energy + c.potential_energy;
    conserved_ = c;
}

}  // namespace physics::core
