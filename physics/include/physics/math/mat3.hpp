#pragma once

#include <physics/math/vec3.hpp>

// Column-major 3x3 matrix (OpenGL convention): element (row, col) = m[col*3 + row].
// constexpr value type, double precision (DESIGN §8.1). stdlib only.

namespace physics::math {

struct Mat3 {
    double m[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    static constexpr Mat3 identity() { return Mat3{{1, 0, 0, 0, 1, 0, 0, 0, 1}}; }
    // Access by (row, col).
    constexpr double operator()(int row, int col) const { return m[col * 3 + row]; }
};

constexpr Mat3 operator*(const Mat3& a, const Mat3& b) {
    Mat3 r{};
    for (int col = 0; col < 3; ++col) {
        for (int row = 0; row < 3; ++row) {
            double sum = 0.0;
            for (int k = 0; k < 3; ++k) {
                sum += a.m[k * 3 + row] * b.m[col * 3 + k];
            }
            r.m[col * 3 + row] = sum;
        }
    }
    return r;
}

constexpr Vec3 operator*(const Mat3& a, Vec3 v) {
    return {a.m[0] * v.x + a.m[3] * v.y + a.m[6] * v.z,
            a.m[1] * v.x + a.m[4] * v.y + a.m[7] * v.z,
            a.m[2] * v.x + a.m[5] * v.y + a.m[8] * v.z};
}

constexpr Mat3 transpose(const Mat3& a) {
    Mat3 r{};
    for (int col = 0; col < 3; ++col) {
        for (int row = 0; row < 3; ++row) {
            r.m[col * 3 + row] = a.m[row * 3 + col];
        }
    }
    return r;
}

constexpr double determinant(const Mat3& a) {
    const double m00 = a.m[0], m10 = a.m[1], m20 = a.m[2];
    const double m01 = a.m[3], m11 = a.m[4], m21 = a.m[5];
    const double m02 = a.m[6], m12 = a.m[7], m22 = a.m[8];
    return m00 * (m11 * m22 - m12 * m21) - m01 * (m10 * m22 - m12 * m20) +
           m02 * (m10 * m21 - m11 * m20);
}

// Inverse via the adjugate. Returns the zero matrix for a singular input.
constexpr Mat3 inverse(const Mat3& a) {
    const double det = determinant(a);
    if (det == 0.0) {
        return Mat3{};
    }
    const double inv_det = 1.0 / det;
    const double m00 = a.m[0], m10 = a.m[1], m20 = a.m[2];
    const double m01 = a.m[3], m11 = a.m[4], m21 = a.m[5];
    const double m02 = a.m[6], m12 = a.m[7], m22 = a.m[8];

    Mat3 r{};
    // r(row, col) = cofactor / det, stored column-major.
    r.m[0] = (m11 * m22 - m12 * m21) * inv_det;   // (0,0)
    r.m[1] = -(m10 * m22 - m12 * m20) * inv_det;  // (1,0)
    r.m[2] = (m10 * m21 - m11 * m20) * inv_det;   // (2,0)
    r.m[3] = -(m01 * m22 - m02 * m21) * inv_det;  // (0,1)
    r.m[4] = (m00 * m22 - m02 * m20) * inv_det;   // (1,1)
    r.m[5] = -(m00 * m21 - m01 * m20) * inv_det;  // (2,1)
    r.m[6] = (m01 * m12 - m02 * m11) * inv_det;   // (0,2)
    r.m[7] = -(m00 * m12 - m02 * m10) * inv_det;  // (1,2)
    r.m[8] = (m00 * m11 - m01 * m10) * inv_det;   // (2,2)
    return r;
}

}  // namespace physics::math
