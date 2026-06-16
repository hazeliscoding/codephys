#include "ch03_projectile.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <string>

#include <physics/core/integrator.hpp>
#include <physics/math/constants.hpp>
#include <physics/math/units.hpp>
#include <render/color.hpp>
#include <ui/ui.hpp>

namespace app {

using physics::core::Integrator;
using physics::core::Particle;
using physics::math::Vec2;

namespace {
constexpr std::array<Integrator, 4> kIntegrators{Integrator::ExplicitEuler,
                                                 Integrator::SemiImplicitEuler,
                                                 Integrator::Verlet, Integrator::RK4};
constexpr std::array<const char*, 4> kIntegratorNames{"Euler", "Semi-Euler", "Verlet", "RK4"};
}  // namespace

ProjectileScene::ProjectileScene() {
    reset();
}

double ProjectileScene::analytic_range() const {
    const double a = physics::math::deg_to_rad(angle_deg_);
    return v0_mps_ * v0_mps_ * std::sin(2.0 * a) / gravity_;  // College Physics 2e §3.4
}

double ProjectileScene::analytic_height() const {
    const double a = physics::math::deg_to_rad(angle_deg_);
    const double vy = v0_mps_ * std::sin(a);
    return vy * vy / (2.0 * gravity_);
}

void ProjectileScene::reset() {
    const double a = physics::math::deg_to_rad(angle_deg_);
    const double g = gravity_;

    world_.clear();
    world_.set_integrator(kIntegrators[static_cast<std::size_t>(integrator_index_)]);
    world_.set_acceleration([g](Vec2, Vec2, double) { return Vec2{0.0, -g}; });
    world_.set_potential_energy(
        [g](const Particle& p) { return p.mass * g * std::max(p.position.y, 0.0); });
    world_.add(Particle{Vec2{0.0, 0.0}, Vec2{v0_mps_ * std::cos(a), v0_mps_ * std::sin(a)},
                        1.0, Vec2{}});

    landed_ = false;
    prev_pos_ = Vec2{0.0, 0.0};
    curr_pos_ = Vec2{0.0, 0.0};
    path_.clear();
    path_.push_back(curr_pos_);
    energy_history_.clear();
}

void ProjectileScene::update(double dt) {
    if (landed_) {
        return;
    }
    Particle& p = world_.particles().front();
    prev_pos_ = p.position;
    world_.step(dt);
    curr_pos_ = p.position;

    if (curr_pos_.y <= 0.0 && p.velocity.y < 0.0) {
        curr_pos_.y = 0.0;
        p.position.y = 0.0;
        landed_ = true;
    }
    if (path_.size() < 5000) {
        path_.push_back(curr_pos_);
    }
    if (energy_history_.size() < 600) {
        energy_history_.push_back(world_.conserved().total_energy);
    }
}

render::Camera2D ProjectileScene::camera(int fb_w, int fb_h) const {
    const double range = std::max(analytic_range(), 1.0);
    const double height = std::max(analytic_height(), 1.0);
    const double margin = 0.15 * std::max(range, height) + 1.0;
    const double need_w = range + 2.0 * margin;
    const double need_h = height + 2.0 * margin;
    const double aspect = fb_h > 0 ? static_cast<double>(fb_w) / fb_h : 1.6;

    render::Camera2D cam;
    cam.viewport_width = fb_w;
    cam.viewport_height = fb_h;
    cam.world_height = std::max(need_h, need_w / aspect);
    cam.center = Vec2{range * 0.5, cam.world_height * 0.5 - margin};
    view_ = cam;  // cache for world->screen label placement in render()
    return cam;
}

void ProjectileScene::render(render::Renderer& renderer, double alpha) {
    using namespace render;
    const double range = analytic_range();
    const double height = analytic_height();
    const double scale_ref = std::max(range, height);
    const double radius = std::max(0.02 * scale_ref, 0.1);
    const Color guide{0.40f, 0.44f, 0.52f, 1.0f};
    const float label_scale = 1.6f;

    // A small helper to drop a world-anchored text label (screen-space).
    auto world_label = [&](Vec2 world, const char* text, Color color, double dx, double dy) {
        const Vec2 s = view_.world_to_screen(world);
        renderer.text(Vec2{s.x + dx, s.y + dy}, text, color, label_scale);
    };

    // Ground.
    renderer.line2D(Vec2{-5.0, 0.0}, Vec2{range + 10.0, 0.0}, colors::gray, 1.5);

    // Guide lines tying the picture to the numbers: range (x) and apex (max height).
    renderer.line2D(Vec2{range, 0.0}, Vec2{range, height}, guide, 1.0);
    renderer.line2D(Vec2{0.0, height}, Vec2{range, height}, guide, 1.0);
    world_label(Vec2{range, 0.0}, "range", guide, 6.0, -22.0);
    world_label(Vec2{0.0, height}, "max height", guide, 6.0, -18.0);
    world_label(Vec2{0.0, 0.0}, "launch", colors::gray, 6.0, 6.0);

    // Trajectory trail.
    for (std::size_t i = 1; i < path_.size(); ++i) {
        renderer.line2D(path_[i - 1], path_[i], colors::yellow, 1.5);
    }

    // Interpolated projectile position.
    const Vec2 pos{prev_pos_.x + (curr_pos_.x - prev_pos_.x) * alpha,
                   prev_pos_.y + (curr_pos_.y - prev_pos_.y) * alpha};
    renderer.circle(pos, radius, colors::red, true);
    world_label(pos, "ball", colors::red, radius * 0.0 + 10.0, -8.0);

    if (show_velocity_ && !landed_) {
        const Vec2 vel = world_.particles().front().velocity;
        const double vscale = 0.25 * scale_ref / std::max(v0_mps_, 1.0);
        const Vec2 tip = pos + vel * vscale;
        renderer.arrow2D(pos, tip, colors::green, 2.0);
        world_label(tip, "velocity", colors::green, 6.0, -8.0);
    }
}

void ProjectileScene::ui(const ui::InputState&) {
    ui::heading("Projectile");
    ui::help("A ball thrown at an angle; gravity makes a parabola.");
    ui::separator();

    bool changed = false;
    changed |= ui::sliderFloat("speed (m/s)", &v0_mps_, 5.0, 60.0);
    changed |= ui::sliderFloat("angle (deg)", &angle_deg_, 5.0, 85.0);
    changed |= ui::sliderFloat("g (m/s2)", &gravity_, 1.0, 25.0);

    ui::checkbox("show velocity", &show_velocity_);

    if (ui::button(std::string("Method: ") +
                   kIntegratorNames[static_cast<std::size_t>(integrator_index_)])) {
        integrator_index_ = (integrator_index_ + 1) % 4;
        changed = true;
    }
    if (ui::button("Fire")) {
        changed = true;
    }

    ui::separator();
    ui::labelEquation("R = v0^2 sin(2T)/g");
    ui::help("Formula vs simulation - they match.");

    char buf[48] = {};
    std::snprintf(buf, sizeof(buf), "predicted: %.1f m", analytic_range());
    ui::label(buf);
    std::snprintf(buf, sizeof(buf), "measured: %.1f m", curr_pos_.x);
    ui::label(buf);
    std::snprintf(buf, sizeof(buf), "max height: %.1f m", analytic_height());
    ui::label(buf);
    std::snprintf(buf, sizeof(buf), "energy: %.0f J", world_.conserved().total_energy);
    ui::label(buf);
    ui::plotLine("Energy (J)", energy_history_, 50.0f);

    if (changed) {
        reset();
    }
}

}  // namespace app
