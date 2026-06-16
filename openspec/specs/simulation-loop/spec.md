# simulation-loop

## Purpose

Drive the simulation with a frame-rate-independent fixed-timestep accumulator loop that
interpolates rendering between physics ticks, and expose user time controls (play, pause,
step, reset) plus time-scale and timestep selection.

## Requirements

### Requirement: Fixed-timestep loop with interpolation
The application SHALL drive simulation with a fixed-timestep accumulator loop: each frame
it adds the (clamped) elapsed wall time to an accumulator, runs `World::step(dt)` while
the accumulator holds at least one `dt`, and renders the state interpolated between the
two most recent physics states by `alpha = accumulator / dt`. Frame time SHALL be clamped
to prevent an unbounded catch-up spiral.

#### Scenario: Physics is frame-rate independent
- **WHEN** the same scene runs at different rendering frame rates
- **THEN** the simulated trajectory over a given wall-clock interval is equivalent (within interpolation)

#### Scenario: Rendering interpolates between ticks
- **WHEN** a frame is presented with the accumulator partway between ticks
- **THEN** the drawn state is the interpolation of the previous and current physics states

### Requirement: Time control
The application SHALL expose Play, Pause, Step-one-tick, and Reset controls. Pause SHALL
halt accumulation; Step SHALL advance exactly one `dt` while paused; Reset SHALL restore
the active scene to its initial conditions.

#### Scenario: Step advances exactly one tick while paused
- **WHEN** the simulation is paused and Step is activated
- **THEN** the world advances by exactly one `dt` and then halts again

#### Scenario: Reset restores initial conditions
- **WHEN** Reset is activated
- **THEN** the active scene returns to its initial state and conserved-quantity history is cleared

### Requirement: Time-scale and timestep selection
The application SHALL provide a time-scale control (slow-motion through fast-forward) and
a `dt` selector. Changing `dt` SHALL take effect for subsequent ticks so the user can
observe a scheme's numerical stability change with timestep.

#### Scenario: Time-scale changes simulated rate without changing dt
- **WHEN** the time-scale is increased
- **THEN** more physics ticks run per wall-clock second while each tick still uses the selected `dt`

#### Scenario: Larger dt visibly affects stability
- **WHEN** the user increases `dt` for an integrator prone to instability
- **THEN** the simulation reflects the larger step (e.g., visible energy gain for Explicit Euler)
