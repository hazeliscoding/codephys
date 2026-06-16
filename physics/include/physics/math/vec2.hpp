#pragma once

#include <cmath>

// 2D vector — constexpr value type, double precision (DESIGN §8.1). physics/ depends on
// the C++ standard library ONLY; no GLFW/OpenGL/render/ui includes here.

namespace physics::math {

struct Vec2 {
    double x = 0.0;
    double y = 0.0;
};

constexpr bool operator==(Vec2 a, Vec2 b) {
    return a.x == b.x && a.y == b.y;
}
constexpr bool operator!=(Vec2 a, Vec2 b) {
    return !(a == b);
}

constexpr Vec2 operator+(Vec2 a, Vec2 b) {
    return {a.x + b.x, a.y + b.y};
}
constexpr Vec2 operator-(Vec2 a, Vec2 b) {
    return {a.x - b.x, a.y - b.y};
}
constexpr Vec2 operator-(Vec2 v) {
    return {-v.x, -v.y};
}
constexpr Vec2 operator*(Vec2 v, double s) {
    return {v.x * s, v.y * s};
}
constexpr Vec2 operator*(double s, Vec2 v) {
    return v * s;
}
constexpr Vec2 operator/(Vec2 v, double s) {
    return {v.x / s, v.y / s};
}

constexpr Vec2& operator+=(Vec2& a, Vec2 b) {
    a = a + b;
    return a;
}
constexpr Vec2& operator-=(Vec2& a, Vec2 b) {
    a = a - b;
    return a;
}
constexpr Vec2& operator*=(Vec2& a, double s) {
    a = a * s;
    return a;
}
constexpr Vec2& operator/=(Vec2& a, double s) {
    a = a / s;
    return a;
}

constexpr double dot(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}
// 2D cross product is the scalar z-component of the 3D cross (a.x*b.y - a.y*b.x).
constexpr double cross(Vec2 a, Vec2 b) {
    return a.x * b.y - a.y * b.x;
}
constexpr double length_squared(Vec2 v) {
    return dot(v, v);
}
inline double length(Vec2 v) {
    return std::sqrt(length_squared(v));
}
inline Vec2 normalize(Vec2 v) {
    const double len = length(v);
    return len > 0.0 ? v / len : Vec2{};
}

}  // namespace physics::math
