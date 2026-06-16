#pragma once

#include <vector>

#include <physics/core/world.hpp>
#include <physics/math/vec2.hpp>

#include "../scene.hpp"

// Projectile motion (College Physics 2e Ch 3). A single particle under uniform gravity,
// with live controls and readouts; its range matches R = v0^2 sin(2θ)/g.

namespace app {

class ProjectileScene : public Scene {
public:
    ProjectileScene();
    void reset() override;
    void update(double dt) override;
    render::Camera2D camera(int fb_w, int fb_h) const override;
    void render(render::Renderer& renderer, double alpha) override;
    void ui(const ui::InputState& input) override;
    std::string_view name() const override { return "Projectile"; }

private:
    double analytic_range() const;
    double analytic_height() const;

    physics::core::World world_;
    double v0_mps_ = 30.0;
    double angle_deg_ = 45.0;
    double gravity_ = 9.80665;
    bool show_velocity_ = true;
    int integrator_index_ = 3;  // RK4 by default

    bool landed_ = false;
    physics::math::Vec2 prev_pos_{};
    physics::math::Vec2 curr_pos_{};
    std::vector<physics::math::Vec2> path_;
    std::vector<double> energy_history_;
    mutable render::Camera2D view_{};  // cached each frame for world->screen labels
};

}  // namespace app
