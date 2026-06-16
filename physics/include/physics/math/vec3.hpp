#pragma once

#include <cmath>

// 3D vector — constexpr value type, double precision (DESIGN §8.1). stdlib only.

namespace physics::math {

struct Vec3 {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

constexpr bool operator==(Vec3 a, Vec3 b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}
constexpr bool operator!=(Vec3 a, Vec3 b) {
    return !(a == b);
}

constexpr Vec3 operator+(Vec3 a, Vec3 b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}
constexpr Vec3 operator-(Vec3 a, Vec3 b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
constexpr Vec3 operator-(Vec3 v) {
    return {-v.x, -v.y, -v.z};
}
constexpr Vec3 operator*(Vec3 v, double s) {
    return {v.x * s, v.y * s, v.z * s};
}
constexpr Vec3 operator*(double s, Vec3 v) {
    return v * s;
}
constexpr Vec3 operator/(Vec3 v, double s) {
    return {v.x / s, v.y / s, v.z / s};
}

constexpr Vec3& operator+=(Vec3& a, Vec3 b) {
    a = a + b;
    return a;
}
constexpr Vec3& operator-=(Vec3& a, Vec3 b) {
    a = a - b;
    return a;
}
constexpr Vec3& operator*=(Vec3& a, double s) {
    a = a * s;
    return a;
}
constexpr Vec3& operator/=(Vec3& a, double s) {
    a = a / s;
    return a;
}

constexpr double dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
constexpr Vec3 cross(Vec3 a, Vec3 b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
constexpr double length_squared(Vec3 v) {
    return dot(v, v);
}
inline double length(Vec3 v) {
    return std::sqrt(length_squared(v));
}
inline Vec3 normalize(Vec3 v) {
    const double len = length(v);
    return len > 0.0 ? v / len : Vec3{};
}

}  // namespace physics::math
