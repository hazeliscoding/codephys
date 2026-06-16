// The physics math module is header-only (constexpr value types). This TU exists so the
// headers are compiled as part of the `physics` static library and any future non-inline
// math helpers have a home. Core (Particle/World/integrators) adds its own sources.
#include <physics/math.hpp>
