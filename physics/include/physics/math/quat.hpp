#pragma once

#include <cmath>

#include <physics/math/vec3.hpp>

// Quaternion (x, y, z, w) — constexpr value type, double precision (DESIGN §8.1).
// Used for 3D orientation in a later phase; included now for a complete math module.
// stdlib only.

namespace physics::math {

struct Quat {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    double w = 1.0;  // identity rotation

    static constexpr Quat identity() { return Quat{0.0, 0.0, 0.0, 1.0}; }
};

constexpr bool operator==(const Quat& a, const Quat& b) {
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

// Hamilton product (composition of rotations).
constexpr Quat operator*(const Quat& a, const Quat& b) {
    return {a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
            a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
            a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
            a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z};
}

constexpr Quat conjugate(const Quat& q) {
    return {-q.x, -q.y, -q.z, q.w};
}
constexpr double norm_squared(const Quat& q) {
    return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
}
inline double norm(const Quat& q) {
    return std::sqrt(norm_squared(q));
}
inline Quat normalize(const Quat& q) {
    const double n = norm(q);
    return n > 0.0 ? Quat{q.x / n, q.y / n, q.z / n, q.w / n} : Quat::identity();
}

}  // namespace physics::math
