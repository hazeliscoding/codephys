#pragma once

#include <cmath>

#include <physics/math/vec3.hpp>

// 4D vector — constexpr value type, double precision (DESIGN §8.1). Used mainly for
// homogeneous coordinates with Mat4. stdlib only.

namespace physics::math {

struct Vec4 {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    double w = 0.0;
};

constexpr bool operator==(Vec4 a, Vec4 b) {
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}
constexpr bool operator!=(Vec4 a, Vec4 b) {
    return !(a == b);
}

constexpr Vec4 operator+(Vec4 a, Vec4 b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}
constexpr Vec4 operator-(Vec4 a, Vec4 b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}
constexpr Vec4 operator*(Vec4 v, double s) {
    return {v.x * s, v.y * s, v.z * s, v.w * s};
}
constexpr Vec4 operator*(double s, Vec4 v) {
    return v * s;
}

constexpr double dot(Vec4 a, Vec4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// Homogeneous helpers.
constexpr Vec4 to_point(Vec3 v) {
    return {v.x, v.y, v.z, 1.0};
}
constexpr Vec3 xyz(Vec4 v) {
    return {v.x, v.y, v.z};
}

}  // namespace physics::math
