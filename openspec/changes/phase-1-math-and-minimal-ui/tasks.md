## 1. physics::math

- [x] 1.1 Add `Vec2`/`Vec3`/`Vec4` `constexpr` value types (aggregate, `double`) with add/sub/scale, `dot`, `cross` (3D), `length`, `normalize`
- [x] 1.2 Add `Mat3`/`Mat4` with multiply, `transpose`, `inverse`, identity; add `Quat` with multiply + normalize
- [x] 1.3 Add `constants.hpp` with SI constants (`G`, `g_earth`, `c`, `k_e`, `eps0`, `mu0`, `h`, `k_B`), each with a `College Physics 2e` citation
- [x] 1.4 Add `units.hpp` centralizing display conversions (deg↔rad at minimum)
- [x] 1.5 Remove the Phase 0 `Vec2` placeholder and update its references (`render/src/render.cpp`, `tests/test_math.cpp`)
- [x] 1.6 Update the `physics`/`physics::math` target wiring so `render` consumes the math surface; keep `physics` stdlib-only
- [x] 1.7 Unit tests: dot/cross/normalize, matrix inverse round-trip, quaternion, deg/rad round-trip (headless)

## 2. physics::core — Particle, World, integrators

- [x] 2.1 Add `Particle` (position, velocity, mass, force/accel accumulator) in SI, depending only on stdlib + `physics::math`
- [x] 2.2 Define the acceleration-function interface `a(position, velocity, t)` and the `Integrator` enum
- [x] 2.3 Implement Explicit Euler and Semi-implicit Euler integrators
- [x] 2.4 Implement Velocity Verlet and RK4 integrators
- [x] 2.5 Add `World` owning particles with `step(dt)`: clear accumulators → compute acceleration → integrate (selected scheme) → record conserved quantities
- [x] 2.6 Expose per-tick conserved quantities (KE, PE, linear momentum, angular momentum)
- [x] 2.7 Make the active integrator runtime-switchable on `World`

## 3. physics validation tests (headless)

- [x] 3.1 Analytic: projectile range `R = v0²·sin(2θ)/g`, max height, time of flight (RK4, small dt)
- [x] 3.2 Analytic: circular-orbit period vs Kepler's third law
- [x] 3.3 Convergence-order tests: global error vs dt and dt/2 (Euler ≈ O(dt), RK4 ≈ O(dt⁴))
- [x] 3.4 Invariants: symplectic energy bound over many periods (semi-implicit Euler, Verlet); momentum & angular momentum conservation
- [x] 3.5 Determinism: same ICs + dt + integrator produce identical trajectories

## 4. render — 2D renderer, camera, text

- [x] 4.1 Implement `Renderer` core: batched dynamic VBO + begin/end frame; flat-color GLSL 3.30 program(s) for triangles and lines
- [x] 4.2 Implement primitives: `line2D` (with width), `circle`, `polygon` (filled/outline), `arrow2D`
- [x] 4.3 Implement `Camera2D` (pan/zoom ortho) producing view-projection; add world↔screen conversion helpers
- [x] 4.4 Vendor/embed a public-domain 8×8 ASCII bitmap font (record source, glad-README style); upload as an R8 atlas
- [x] 4.5 Implement `text(screenPos, string, color)` + a text-measurement query
- [x] 4.6 Smoke-render all primitives + text in a temporary harness; confirm no GL errors on Windows and Linux/WSL <!-- verified via the scene gallery: GL-clean on Windows (Intel Arc) and Linux/WSLg (llvmpipe) -->

## 5. ui — immediate-mode toolkit

- [x] 5.1 Define the `InputState` snapshot (mouse pos, buttons, pressed keys, scroll) and the per-frame UI context/layout cursor
- [x] 5.2 Implement hot/active-item tracking (IDs from label/call order) drawing through `Renderer`
- [x] 5.3 Implement widgets: `SliderFloat`, `Button`, `Checkbox`, `Label`, `LabelEquation`, `PlotLine`
- [x] 5.4 Confirm `ui` links/includes no GLFW/OpenGL/platform code (layering canary)

## 6. app — fixed-timestep loop & time control

- [x] 6.1 In `platform`, expose per-frame input (build the `InputState` snapshot) without leaking GLFW upward
- [x] 6.2 Implement the accumulator loop with clamped frame time and previous/current state interpolation (`alpha`)
- [x] 6.3 Implement time control: Play/Pause, Step-one-tick (while paused), Reset
- [x] 6.4 Implement time-scale control and a `dt` selector affecting subsequent ticks

## 7. scenes — framework & flagship scenes

- [x] 7.1 Define the `Scene` interface (reset/update/render/ui/dimension/name) and `SceneManager` with a gallery + runtime switching
- [x] 7.2 Replace the Phase 0 triangle in `app/main.cpp` with the `SceneManager`-driven loop
- [x] 7.3 Implement `ch03_projectile` scene: uniform-gravity acceleration, controls (v0, angle, g), trajectory + readouts
- [x] 7.4 Implement the integrator-comparison scene: same circular-orbit ICs under Euler vs RK4, drawn together (inline inverse-square accel)
- [x] 7.5 Wire `ui::PlotLine` energy history into a scene readout

## 8. Verification, cross-platform & wrap-up

- [x] 8.1 Build warnings-clean and run `ctest` headless on Windows (MSVC) and Linux/WSL (GCC) <!-- both green; 10 cases / 53 assertions -->
- [x] 8.2 Run the app on both platforms: gallery loads, projectile + comparison scenes render, controls work, clean shutdown <!-- Windows: GL 3.3 Intel Arc; Linux/WSLg: GL 4.5 llvmpipe; both GL-clean -->
- [ ] 8.3 Confirm CI is green on all three legs (windows-msvc, ubuntu-gcc, ubuntu-clang) <!-- blocked: GitHub remote deleted by user (to be reinitialized); ci.yml unchanged & correct -->
- [x] 8.4 Update `README.md` (Phase 1 status, scene list, controls) and `docs/design/DESIGN.md` status/notes
- [x] 8.5 Run `openspec validate phase-1-math-and-minimal-ui` and resolve any issues
