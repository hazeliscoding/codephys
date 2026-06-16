#pragma once

#include <physics/math/mat4.hpp>
#include <physics/math/vec2.hpp>
#include <physics/math/vec4.hpp>

// 2D orthographic camera (DESIGN §7.2): pan via `center`, zoom via `world_height` (the
// number of world units visible vertically). Screen space is pixels with the origin at the
// top-left, x right, y down. Depends only on physics::math.

namespace render {

using physics::math::Mat4;
using physics::math::Vec2;

struct Camera2D {
    Vec2 center{0.0, 0.0};       // world point mapped to the viewport center
    double world_height = 10.0;  // world units visible vertically (smaller = zoomed in)
    int viewport_width = 1;      // px
    int viewport_height = 1;     // px

    double aspect() const {
        return viewport_height > 0
                   ? static_cast<double>(viewport_width) / static_cast<double>(viewport_height)
                   : 1.0;
    }
    double world_width() const { return world_height * aspect(); }

    // World -> clip projection (y up), for the shape shader's u_view_proj uniform.
    Mat4 view_projection() const {
        const double half_h = world_height * 0.5;
        const double half_w = world_width() * 0.5;
        return physics::math::ortho(center.x - half_w, center.x + half_w, center.y - half_h,
                                    center.y + half_h, -1.0, 1.0);
    }

    Vec2 world_to_screen(Vec2 world) const {
        const physics::math::Vec4 clip =
            view_projection() * physics::math::Vec4{world.x, world.y, 0.0, 1.0};
        const double ndc_x = clip.x;  // w == 1 for orthographic
        const double ndc_y = clip.y;
        return Vec2{(ndc_x * 0.5 + 0.5) * viewport_width,
                    (1.0 - (ndc_y * 0.5 + 0.5)) * viewport_height};
    }

    Vec2 screen_to_world(Vec2 screen) const {
        const double ndc_x = screen.x / viewport_width * 2.0 - 1.0;
        const double ndc_y = (1.0 - screen.y / viewport_height) * 2.0 - 1.0;
        const physics::math::Vec4 world = physics::math::inverse(view_projection()) *
                                          physics::math::Vec4{ndc_x, ndc_y, 0.0, 1.0};
        return Vec2{world.x, world.y};
    }
};

}  // namespace render
