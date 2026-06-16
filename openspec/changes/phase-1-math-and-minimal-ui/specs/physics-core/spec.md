## ADDED Requirements

### Requirement: Particle point mass
The core SHALL provide a `Particle` representing a point mass with position, velocity,
mass, and a force/acceleration accumulator, expressed in SI units. `Particle` SHALL
depend only on the standard library and `physics::math`.

#### Scenario: Particle holds SI state
- **WHEN** a `Particle` is constructed with a position, velocity, and mass
- **THEN** those values are stored in SI units and readable for integration and readouts

### Requirement: World step pipeline
The core SHALL provide a `World` that owns particles and exposes `step(dt)`, which each
tick clears accumulators, computes acceleration, integrates the selected scheme, and
records conserved quantities. The pipeline SHALL be structured so future force
generators can be added without changing the `step` contract.

#### Scenario: One tick advances the world by dt
- **WHEN** `World::step(dt)` is called
- **THEN** every particle's state advances by exactly one timestep `dt`
- **AND** the recorded conserved quantities are updated for that tick

### Requirement: Four switchable integrators
The core SHALL implement Explicit (Forward) Euler, Semi-implicit (symplectic) Euler,
Velocity Verlet, and RK4 behind a single interface selectable at runtime, advancing a
particle's `(position, velocity)` given an acceleration function `a(position, velocity, t)`.

#### Scenario: Integrator is selectable at runtime
- **WHEN** the active integrator is changed between ticks
- **THEN** subsequent `step(dt)` calls use the newly selected scheme

#### Scenario: Convergence order matches the scheme
- **WHEN** a known system is integrated at timestep `dt` and again at `dt/2`
- **THEN** the global error decreases consistent with the scheme's order (Euler ≈ O(dt), RK4 ≈ O(dt⁴))

#### Scenario: Symplectic schemes conserve energy within tolerance
- **WHEN** an oscillator or circular orbit is integrated with semi-implicit Euler or Velocity Verlet over many periods
- **THEN** total mechanical energy stays within a bounded tolerance (no secular blow-up)

### Requirement: Conserved-quantity reporting
The core SHALL expose, per tick, the system's kinetic energy, potential energy, linear
momentum, and angular momentum, so scenes and tests can read and assert them.

#### Scenario: Momentum is conserved with no external force
- **WHEN** an isolated system with no net external force is stepped
- **THEN** the reported total linear momentum is conserved within tolerance

### Requirement: Deterministic simulation
Given identical initial conditions, timestep, and integrator, repeated runs SHALL
produce identical trajectories.

#### Scenario: Repeated runs match
- **WHEN** the same scenario is stepped twice with the same dt and integrator
- **THEN** the resulting particle states are bitwise-identical at each tick
