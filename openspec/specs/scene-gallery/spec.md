# scene-gallery

## Purpose

Define the `Scene` interface and a `SceneManager` gallery that registers and switches among
interactive simulations, including a Projectile scene validated against closed-form
solutions and an integrator-comparison demo that makes numerical-scheme divergence visible.

## Requirements

### Requirement: Scene interface
The application SHALL define a `Scene` interface with at least: reset, update(dt),
render(Renderer&), ui(InputState), a dimension query, and a name. Scenes SHALL drive
physics through `physics`/`physics::core` and draw through `render`/`ui` only.

#### Scenario: A scene runs through the loop
- **WHEN** a registered scene is active
- **THEN** the loop calls its update each tick, its render each frame, and its ui each frame

### Requirement: Scene gallery and switching
The application SHALL provide a `SceneManager` that registers multiple scenes, shows a
gallery to select among them, and switches the active scene at runtime, resetting the
newly selected scene to its initial conditions.

#### Scenario: User switches scenes from the gallery
- **WHEN** the user selects a different scene in the gallery
- **THEN** that scene becomes active and starts from its initial conditions

### Requirement: Projectile scene validated to closed form
The application SHALL include a Projectile scene (College Physics 2e Ch 3) with live
controls (at least launch speed and angle, gravity) and physics readouts. Its simulated
range, maximum height, and time of flight SHALL match the closed-form solutions within
tolerance for an accurate integrator and small `dt`.

#### Scenario: Simulated range matches the analytic range
- **WHEN** the projectile is launched with speed `v0_mps` at angle `angle_rad` under gravity `g`
- **THEN** the simulated horizontal range matches `R = v0² · sin(2·angle) / g` within tolerance

#### Scenario: Controls drive the simulation
- **WHEN** the user changes launch speed, angle, or gravity and fires
- **THEN** the trajectory and readouts update to reflect the new parameters

### Requirement: Integrator-comparison demo
The application SHALL include a scene that runs the same initial conditions (a circular
orbit) under different integrators shown together, so the divergence between schemes
(e.g., Explicit Euler spiraling out vs RK4 staying on orbit) is directly visible.

#### Scenario: Schemes visibly diverge on a circular orbit
- **WHEN** the comparison scene runs Explicit Euler and RK4 from the same circular-orbit initial conditions
- **THEN** both trajectories are drawn together and the energy-gaining scheme visibly departs from the stable one over time
