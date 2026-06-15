#include <physics/math/vec2.hpp>

namespace physics::math {

Vec2 midpoint(Vec2 a, Vec2 b) {
    return {(a.x + b.x) * 0.5, (a.y + b.y) * 0.5};
}

}  // namespace physics::math
