#include <doctest/doctest.h>

#include <physics/math.hpp>

using namespace physics::math;

TEST_CASE("vector operations") {
    SUBCASE("dot and cross (2D)") {
        CHECK(dot(Vec2{1.0, 2.0}, Vec2{3.0, 4.0}) == doctest::Approx(11.0));
        CHECK(cross(Vec2{1.0, 0.0}, Vec2{0.0, 1.0}) == doctest::Approx(1.0));
    }
    SUBCASE("cross (3D) is perpendicular") {
        const Vec3 c = cross(Vec3{1.0, 0.0, 0.0}, Vec3{0.0, 1.0, 0.0});
        CHECK(c == Vec3{0.0, 0.0, 1.0});
    }
    SUBCASE("length and normalize") {
        CHECK(length(Vec3{3.0, 4.0, 0.0}) == doctest::Approx(5.0));
        const Vec3 n = normalize(Vec3{0.0, 3.0, 4.0});
        CHECK(length(n) == doctest::Approx(1.0));
        CHECK(normalize(Vec2{0.0, 0.0}) == Vec2{0.0, 0.0});  // zero stays zero
    }
}

TEST_CASE("matrix inverse round-trips to identity") {
    SUBCASE("Mat3") {
        const Mat3 a{{2, 0, 1, 0, 3, 0, 1, 0, 2}};
        const Mat3 prod = a * inverse(a);
        const Mat3 id = Mat3::identity();
        for (int i = 0; i < 9; ++i) {
            CHECK(prod.m[i] == doctest::Approx(id.m[i]));
        }
    }
    SUBCASE("Mat4") {
        const Mat4 a{{1, 0, 0, 0, 2, 1, 0, 0, 0, 3, 1, 0, 4, 0, 5, 1}};
        const Mat4 prod = a * inverse(a);
        const Mat4 id = Mat4::identity();
        for (int i = 0; i < 16; ++i) {
            CHECK(prod.m[i] == doctest::Approx(id.m[i]));
        }
    }
    SUBCASE("ortho maps corners to clip space") {
        const Mat4 p = ortho(-2.0, 2.0, -1.0, 1.0, -1.0, 1.0);
        const Vec4 r = p * Vec4{2.0, 1.0, 0.0, 1.0};  // top-right -> (1, 1)
        CHECK(r.x == doctest::Approx(1.0));
        CHECK(r.y == doctest::Approx(1.0));
    }
}

TEST_CASE("quaternion") {
    SUBCASE("identity is multiplicative unit") {
        const Quat q{0.1, 0.2, 0.3, 0.5};
        const Quat r = q * Quat::identity();
        CHECK(r == q);
    }
    SUBCASE("normalize yields unit norm") {
        CHECK(norm(normalize(Quat{1.0, 2.0, 3.0, 4.0})) == doctest::Approx(1.0));
    }
}

TEST_CASE("units round-trip and constants are SI") {
    CHECK(rad_to_deg(deg_to_rad(57.0)) == doctest::Approx(57.0));
    CHECK(deg_to_rad(180.0) == doctest::Approx(pi));
    CHECK(g_earth == doctest::Approx(9.80665));  // standard gravity, m/s^2
}
