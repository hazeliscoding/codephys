#pragma once

#include <physics/math/constants.hpp>

// Unit conversions for display/UI boundaries ONLY. Internal computation stays in SI
// (CLAUDE.md); boundary variables carry unit suffixes (angle_rad, angle_deg). stdlib only.

namespace physics::math {

constexpr double deg_to_rad(double angle_deg) {
    return angle_deg * (pi / 180.0);
}
constexpr double rad_to_deg(double angle_rad) {
    return angle_rad * (180.0 / pi);
}

}  // namespace physics::math
