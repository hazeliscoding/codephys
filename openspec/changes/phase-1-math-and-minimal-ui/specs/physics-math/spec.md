## ADDED Requirements

### Requirement: Vector and matrix value types
The math library SHALL provide `constexpr` value types `Vec2`, `Vec3`, `Vec4`, `Mat3`,
`Mat4`, and `Quat` with aggregate initialization and the standard operations needed by
the engine: vector add/subtract/scale, `dot`, `cross` (3D), `length`, `normalize`;
matrix multiply, `transpose`, `inverse`, and identity construction; quaternion multiply
and normalization. The scalar type SHALL be `double`. These types SHALL depend only on
the C++ standard library.

#### Scenario: Vector operations are correct
- **WHEN** `dot`, `cross`, `length`, and `normalize` are evaluated on known inputs
- **THEN** the results match the analytic values within floating-point tolerance
- **AND** a normalized non-zero vector has length 1

#### Scenario: Matrix inverse round-trips
- **WHEN** an invertible `Mat3`/`Mat4` is multiplied by its `inverse`
- **THEN** the product equals the identity matrix within tolerance

#### Scenario: Math types are headlessly usable
- **WHEN** the math headers are compiled into the headless test target
- **THEN** they require no GLFW/OpenGL/render/ui include paths

### Requirement: SI physical constants
The math library SHALL provide physical constants in SI units (at minimum gravitational
constant `G`, standard gravity `g_earth`, speed of light `c`, Coulomb constant `k_e`,
vacuum permittivity `eps0` and permeability `mu0`, Planck constant `h`, Boltzmann
constant `k_B`), each documented with a citation to the textbook units chapter.

#### Scenario: Constants are in SI and cited
- **WHEN** a constant is referenced in engine or test code
- **THEN** its value is the SI value and its declaration carries a `College Physics 2e` citation

### Requirement: Unit conversion for display
The math library SHALL centralize unit conversions used at UI/display boundaries (at
minimum degrees↔radians) in a single header. Internal computation SHALL remain in SI;
conversions SHALL be applied only at boundaries.

#### Scenario: Degree/radian conversion round-trips
- **WHEN** an angle in degrees is converted to radians and back
- **THEN** the original value is recovered within tolerance

### Requirement: Shared math vocabulary for the renderer
The math types SHALL be consumable by the `render` module via the `physics::math`
dependency edge, so renderer and physics share the same vector/matrix types without
`physics` depending on `render`.

#### Scenario: Renderer uses math types without inverting the layering
- **WHEN** `render` is built
- **THEN** it links and includes `physics::math` types
- **AND** `physics` links no `render`/`ui`/GLFW/OpenGL code
