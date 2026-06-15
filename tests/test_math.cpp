#include <doctest/doctest.h>

#include <physics/math/vec2.hpp>

// Phase 0 trivial test: proves the headless harness runs and the physics target links.
// Real analytic-validation tests (projectile range, SHM period, ...) arrive in Phase 1.
TEST_CASE("physics::math placeholder") {
    using physics::math::Vec2;

    SUBCASE("vector addition") {
        const Vec2 sum = Vec2{1.0, 2.0} + Vec2{3.0, 4.0};
        CHECK(sum.x == doctest::Approx(4.0));
        CHECK(sum.y == doctest::Approx(6.0));
    }

    SUBCASE("midpoint") {
        const Vec2 m = physics::math::midpoint(Vec2{0.0, 0.0}, Vec2{2.0, 8.0});
        CHECK(m.x == doctest::Approx(1.0));
        CHECK(m.y == doctest::Approx(4.0));
    }
}
