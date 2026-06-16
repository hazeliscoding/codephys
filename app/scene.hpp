#pragma once

#include <string_view>

#include <render/camera2d.hpp>
#include <render/renderer.hpp>
#include <ui/input.hpp>

// Scene interface (DESIGN §5/§7). A scene owns its physics and draws through render/ui; the
// app drives it via the fixed-timestep loop. Phase 1 scenes are 2D.

namespace app {

enum class Dimension { D2, D3 };

class Scene {
public:
    virtual ~Scene() = default;

    virtual void reset() = 0;            // restore initial conditions
    virtual void update(double dt) = 0;  // advance physics by one tick
    virtual render::Camera2D camera(int fb_w, int fb_h) const = 0;      // view for this frame
    virtual void render(render::Renderer& renderer, double alpha) = 0;  // draw the world
    virtual void ui(const ui::InputState& input) = 0;  // control panel widgets

    virtual Dimension dimension() const { return Dimension::D2; }
    virtual std::string_view name() const = 0;
};

}  // namespace app
