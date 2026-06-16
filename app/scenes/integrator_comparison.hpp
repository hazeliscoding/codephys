#pragma once

#include <vector>

#include <physics/core/world.hpp>
#include <physics/math/vec2.hpp>

#include "../scene.hpp"

// Integrator comparison (DESIGN §8.2): the same circular-orbit initial conditions run under
// Explicit Euler and RK4 side by side, so Euler's energy gain (spiral-out) versus RK4's
// stable orbit is directly visible.

namespace app {

class IntegratorComparisonScene : public Scene {
public:
    IntegratorComparisonScene();
    void reset() override;
    void update(double dt) override;
    render::Camera2D camera(int fb_w, int fb_h) const override;
    void render(render::Renderer& renderer, double alpha) override;
    void ui(const ui::InputState& input) override;
    std::string_view name() const override { return "Integrators"; }

private:
    double mu_ = 1.0;      // GM
    double radius_ = 1.0;  // initial orbit radius

    physics::core::World euler_;
    physics::core::World rk4_;
    std::vector<physics::math::Vec2> euler_trail_;
    std::vector<physics::math::Vec2> rk4_trail_;
    mutable render::Camera2D view_{};  // cached each frame for world->screen labels
};

}  // namespace app
