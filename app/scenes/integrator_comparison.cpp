#include "integrator_comparison.hpp"

#include <cmath>

#include <physics/core/integrator.hpp>
#include <physics/math/constants.hpp>
#include <render/color.hpp>
#include <ui/ui.hpp>

namespace app {

using physics::core::Integrator;
using physics::core::Particle;
using physics::math::Vec2;

namespace {

// Central inverse-square acceleration toward the origin: a = -mu * r / |r|^3.
physics::core::AccelFn central_accel(double mu) {
    return [mu](Vec2 p, Vec2, double) {
        const double r = physics::math::length(p);
        if (r <= 0.0) {
            return Vec2{};
        }
        return p * (-mu / (r * r * r));
    };
}

void seed_orbit(physics::core::World& world, Integrator scheme, double mu, double radius) {
    world.clear();
    world.set_integrator(scheme);
    world.set_acceleration(central_accel(mu));
    const double speed = std::sqrt(mu / radius);  // circular-orbit speed
    world.add(Particle{Vec2{radius, 0.0}, Vec2{0.0, speed}, 1.0, Vec2{}});
}

}  // namespace

IntegratorComparisonScene::IntegratorComparisonScene() {
    reset();
}

void IntegratorComparisonScene::reset() {
    seed_orbit(euler_, Integrator::ExplicitEuler, mu_, radius_);
    seed_orbit(rk4_, Integrator::RK4, mu_, radius_);
    euler_trail_.clear();
    rk4_trail_.clear();
}

void IntegratorComparisonScene::update(double dt) {
    euler_.step(dt);
    rk4_.step(dt);
    if (euler_trail_.size() < 6000) {
        euler_trail_.push_back(euler_.particles().front().position);
    }
    if (rk4_trail_.size() < 6000) {
        rk4_trail_.push_back(rk4_.particles().front().position);
    }
}

render::Camera2D IntegratorComparisonScene::camera(int fb_w, int fb_h) const {
    render::Camera2D cam;
    cam.viewport_width = fb_w;
    cam.viewport_height = fb_h;
    cam.center = Vec2{0.0, 0.0};
    cam.world_height = radius_ * 6.0;  // room for Euler to spiral outward
    view_ = cam;
    return cam;
}

void IntegratorComparisonScene::render(render::Renderer& renderer, double /*alpha*/) {
    using namespace render;
    // Central body.
    renderer.circle(Vec2{0.0, 0.0}, radius_ * 0.08, colors::yellow, true);

    for (std::size_t i = 1; i < euler_trail_.size(); ++i) {
        renderer.line2D(euler_trail_[i - 1], euler_trail_[i], colors::red, 1.5);
    }
    for (std::size_t i = 1; i < rk4_trail_.size(); ++i) {
        renderer.line2D(rk4_trail_[i - 1], rk4_trail_[i], colors::green, 1.5);
    }
    const float label_scale = 1.6f;
    auto world_label = [&](Vec2 world, const char* text, Color color, double dx, double dy) {
        const Vec2 s = view_.world_to_screen(world);
        renderer.text(Vec2{s.x + dx, s.y + dy}, text, color, label_scale);
    };
    world_label(Vec2{0.0, 0.0}, "central mass", colors::yellow, 8.0, 6.0);
    if (!euler_trail_.empty()) {
        renderer.circle(euler_trail_.back(), radius_ * 0.05, colors::red, true);
        world_label(euler_trail_.back(), "Euler", colors::red, 8.0, -8.0);
    }
    if (!rk4_trail_.empty()) {
        renderer.circle(rk4_trail_.back(), radius_ * 0.05, colors::green, true);
        world_label(rk4_trail_.back(), "RK4", colors::green, 8.0, -8.0);
    }
}

void IntegratorComparisonScene::ui(const ui::InputState&) {
    ui::heading("Integrators");
    ui::help(
        "The same orbit computed two ways. Both start identical; the math method "
        "decides how well energy is conserved.");
    ui::separator();
    ui::label("Euler (red)");
    ui::help("Simple but gains energy, so it spirals outward.");
    ui::label("RK4 (green)");
    ui::help("Higher accuracy, so it stays on the orbit.");
    ui::separator();
    ui::labelEquation("a = -mu r / |r|^3");
    ui::help("Gravity toward the central mass.");
    if (ui::button("Reset")) {
        reset();
    }
}

}  // namespace app
