#pragma once

// Physical and mathematical constants, SI units, cross-checked against the textbook's
// units chapter (College Physics 2e §1.2). Names carry no unit suffix because the unit is
// fixed and documented per constant. stdlib only.

namespace physics::math {

// Mathematical.
inline constexpr double pi = 3.14159265358979323846;
inline constexpr double two_pi = 2.0 * pi;
inline constexpr double half_pi = pi / 2.0;

// Physical (SI). College Physics 2e §1.2 and the relevant chapters.
inline constexpr double g_earth = 9.80665;        // standard gravity, m/s^2  (§1.2)
inline constexpr double G = 6.67430e-11;          // gravitational constant, N·m^2/kg^2 (§6.5)
inline constexpr double c_light = 2.99792458e8;   // speed of light, m/s (§1.2)
inline constexpr double k_e = 8.9875517923e9;     // Coulomb constant, N·m^2/C^2 (§18.3)
inline constexpr double eps0 = 8.8541878128e-12;  // vacuum permittivity, F/m (§18.3)
inline constexpr double mu0 = 1.25663706212e-6;   // vacuum permeability, T·m/A (§22.4)
inline constexpr double h_planck = 6.62607015e-34;  // Planck constant, J·s (§29.1)
inline constexpr double k_B = 1.380649e-23;         // Boltzmann constant, J/K (§13.3)

}  // namespace physics::math
