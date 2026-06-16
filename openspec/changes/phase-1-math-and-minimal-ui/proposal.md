## Why

Phase 0 proved the cross-platform GL pipeline but contains no physics, no reusable
renderer, and no UI — the app just draws a throwaway triangle. This change is **Phase 1**
of the roadmap (DESIGN.md §10): it builds the engine's foundation — the math library, the
deterministic simulation loop, the switchable integrators, a 2D renderer, and an
immediate-mode UI — and proves the whole stack works end to end with the **flagship
Projectile scene (Ch 3)** validated against its closed-form solution. After this phase,
every later chapter is "add a force generator / a scene," not "build infrastructure."

## What Changes

- **`physics::math`** — hand-rolled `constexpr` value types (`Vec2/3/4`, `Mat3/4`, `Quat`),
  SI physical `constants`, and a centralized unit-conversion header. Replaces the Phase 0
  `Vec2` placeholder. Fully unit-tested.
- **`physics::core`** — `Particle` (point mass) and `World` (owns bodies, runs
  `step(dt)`); the four switchable integrators behind one interface (Explicit Euler,
  Semi-implicit Euler, Velocity Verlet, RK4); per-step conserved-quantity hooks (KE, PE,
  momentum, angular momentum). Validated against analytic solutions and invariants.
- **Fixed-timestep simulation loop** — accumulator-based stepping with render
  interpolation (DESIGN §7.1), plus time control: Play / Pause / Step-one-tick / Reset, a
  time-scale slider, and a `dt` selector (so cranking `dt` until a scheme blows up is a
  visible lesson). Replaces the Phase 0 bare render loop.
- **2D renderer** — a renderer-agnostic `Renderer` over OpenGL 3.3 with batched 2D
  primitives (`line2D`, `circle`, `polygon`, `arrow2D`), a `Camera2D` (pan/zoom ortho),
  and `text` via an embedded hand-rolled bitmap font (the typography deferred from Phase 0).
- **Immediate-mode UI (`ui::`)** — a tiny retained-free widget set drawing through
  `Renderer` and reading GLFW input: `SliderFloat`, `Button`, `Checkbox`, `Label` /
  `LabelEquation`, `PlotLine`.
- **Scene framework + flagship scenes** — a `Scene` interface and a `SceneManager`
  gallery; the **Projectile scene (Ch 3)** with live controls and readouts, and an
  **integrator-comparison demo** (Euler vs RK4 on a circular orbit, side by side).

## Capabilities

### New Capabilities
- `physics-math`: Vectors/matrices/quaternion value types, SI constants, and unit
  conversions — the shared math vocabulary for physics and render.
- `physics-core`: `Particle`, `World`, the four switchable integrators, and
  conserved-quantity reporting — the deterministic simulation core.
- `simulation-loop`: Fixed-timestep stepping with render interpolation and user-facing
  time control (play/pause/step/reset/time-scale/dt).
- `renderer-2d`: Batched 2D drawing primitives, `Camera2D`, and bitmap-font text over
  OpenGL 3.3.
- `ui-toolkit`: Immediate-mode widgets (slider/button/checkbox/label/plot) for scene
  control panels.
- `scene-gallery`: The `Scene` interface, the gallery/`SceneManager`, and the Phase 1
  flagship scenes (projectile + integrator comparison).

### Modified Capabilities
<!-- None. This builds on build-system and platform-window without changing their
     requirements; the existing "module skeleton" requirement already anticipates these
     targets. -->

## Impact

- **New code:** `physics/` (math + core), `render/` (2D renderer, camera, font),
  `ui/` (widgets), `app/` (loop, time control, `SceneManager`, `app/scenes/`), and
  `tests/` (math unit tests; analytic/invariant/convergence physics tests).
- **Replaced:** the Phase 0 `physics::math::Vec2` placeholder and the throwaway triangle
  in `app/main.cpp`.
- **Layering:** exercises the full `app → scenes → {physics, render, ui}`,
  `render → physics::math`, `ui → render` graph for the first time; `physics/` stays
  stdlib-only.
- **Dependencies:** **no new runtime dependency** — GLFW remains the only one. Math,
  renderer, UI, and text (embedded bitmap font) are all hand-rolled; tests use the
  vendored doctest.
- **Testing/CI:** substantially expands the headless test suite (math + physics analytic
  validation), all running on the existing windows-msvc / ubuntu-gcc / ubuntu-clang matrix.

## Non-goals

- **No force generators beyond uniform gravity.** Springs, drag, friction, and Newtonian
  N-body gravity are **Phase 2**. The orbit in the integrator-comparison demo uses an
  inverse-square central force computed inline for the demo, not the general force-generator
  system.
- **No collisions or constraints** (restitution, contacts, pin/distance) — Phase 2.
- **No `RigidBody2D`/rotation** (torque, inertia) — Phase 3. Phase 1 ships `Particle` only.
- **No 3D** renderer, camera, or bodies — Phase 4.
- **No fields, E&M, waves, or modern-physics scenes** — Phases 5–7.
- **No SIMD / performance work** — clarity over speed; the math is the learning material.
- **No full compile-time dimensional-analysis system** — unit suffixes + one conversion
  header only.
- **No Dear ImGui** — the `ui::` facade keeps that as a future swap-in seam, not a Phase 1
  dependency.
