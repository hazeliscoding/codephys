# CodePhys — Design Document

A cross-platform C++ physics **simulation playground**. The goal is two-fold and
deliberately personal: **(1) relearn introductory physics from the ground up** by
implementing it, and **(2) render awesome, interactive simulations** you can poke,
tweak, and watch in real time.

The physics curriculum is grounded in **OpenStax _College Physics 2e_** (the textbook
in `docs/Textbooks/`), all 34 chapters from kinematics through modern physics.

> Status: **DESIGN / PLANNING**. Nothing is built yet. This doc is the contract for
> what we build and in what order.

---

## 1. Vision & guiding principles

We are not building a competitor to Box2D, Bullet, or PhysX. We are building a
**teaching engine**: code where the physics is legible, the numerics are visible, and
every simulation maps back to a chapter you can re-read.

Three principles drive every decision below:

1. **Learning-first.** When there's a choice between "use a black-box library" and
   "write it ourselves and understand it," we write it ourselves. This is why the
   physics library, the math, the renderer, and even the UI are hand-rolled. The point
   is not to ship fast — it's to *understand*.
2. **Minimal dependencies.** Exactly one third-party runtime library: **GLFW** (window
   + input + OpenGL context). Everything else — vector math, integrators, the
   renderer, the immediate-mode UI, text — is ours. (See §4 for the one pragmatic
   exception we leave open.)
3. **Cross-platform from day one.** Builds and runs identically on **Windows** (MSVC)
   and **Linux / WSL2** (GCC/Clang). CMake is the single source of build truth. No
   platform-specific code outside a thin platform layer.

### Pedagogy as a feature

The thing that makes this a *learning* tool rather than just a demo reel: we instrument
the physics so you can *see* it.

- **Switchable integrators** per scene (Explicit Euler / Semi-implicit Euler / Verlet /
  RK4) so you can watch energy drift differently and understand *why* symplectic
  integrators conserve energy.
- **Live conserved-quantity readouts**: kinetic/potential/total energy, linear &
  angular momentum. When they drift, that's a lesson.
- **Visual overlays**: velocity/acceleration/force vectors, trajectory trails, field
  lines, phase-space plots.
- **Every scene shows its governing equations** in the side panel, tied back to the
  textbook section number.

---

## 2. Goals & non-goals

### Goals
- A single native app: a **gallery** of simulations; pick one, it opens with a live
  **control panel** (sliders/toggles), play/pause/step/reset, and physics readouts.
- A reusable, well-tested **custom physics library** (`physics/`) independent of the
  renderer — it could be driven by a headless test harness with no window.
- A **hybrid renderer** where each scene declares whether it is 2D or 3D; the engine
  provides a shared camera/draw API for both.
- Correctness we can prove: simulations validated against **closed-form analytic
  solutions** (projectile range, SHM period, orbital period, etc.).

### Non-goals (at least initially)
- Photorealism. We want *clear* and *pretty*, not ray-traced.
- A general-purpose game engine, asset pipeline, scripting language, or editor.
- Production-grade collision performance (no broad-phase spatial hashing in v1 — N²
  is fine for the body counts intro physics needs).
- Networking, audio, VR.
- Covering every one of the 34 chapters in code. Many late-modern-physics chapters
  (particle physics, GUTs, cosmology) become *visualizations/explainers*, not dynamical
  simulations — see §9.

---

## 3. Target platforms & toolchain

| Concern | Choice | Notes |
|---|---|---|
| Language | **C++20** | concepts, `<numbers>`, designated initializers, `constexpr` math |
| Build | **CMake ≥ 3.21** | presets for win/linux; single source of truth |
| Compilers | MSVC 19.3x (VS 2022), GCC ≥ 11, Clang ≥ 14 | all warnings-as-errors in CI |
| Windowing/input/GL | **GLFW 3.4** | the *only* runtime dependency |
| Graphics API | **OpenGL 3.3 Core** | ubiquitous, works under WSLg, simplest to hand-roll against |
| GL loader | **glad** (generated, *vendored as source*) | not a package dep — a checked-in generated `.c/.h`. Loads GL function pointers. |
| Math | **hand-rolled** (`physics/math`) | vec2/3/4, mat3/4, quaternion |
| UI | **hand-rolled immediate-mode** | see §7 (and the §4 escape hatch) |
| Text | **hand-rolled bitmap/SDF font** | embedded font atlas, no FreeType |
| Tests | **hand-rolled micro-framework** or single-header (doctest, vendored) | keep deps near zero |

### Why OpenGL 3.3 and not Vulkan/WebGPU
Given "minimal dependencies + hand-rolled," Vulkan would mean thousands of lines of
boilerplate before a single triangle, which fights the learning goal (we want to learn
*physics*, not spend three months on swapchain synchronization). OpenGL 3.3 Core is:
- supported everywhere we target, including **WSLg** (WSL2's GUI support) and over
  remote X if needed,
- minimal to bring up (context from GLFW, load with glad, go),
- enough for everything in §9 (instanced primitives, line rendering, simple lighting).

If we ever outgrow it, the renderer abstraction (§7) is the seam we'd swap behind.

---

## 4. The "minimal dependencies" decision — and one honest caveat

You chose **minimal deps: GLFW only + hand-rolled math/UI/render**. We honor that. The
one place this bites is the **UI**: a usable immediate-mode GUI (sliders, drag-floats,
checkboxes, text, windows) is a few thousand lines and a real time sink that competes
with writing physics.

So the design carries an explicit, isolated **escape hatch**:

- We build a *small* hand-rolled immediate-mode UI (`ui/`) — enough for sliders,
  buttons, checkboxes, labels, and a value plot. This is the default and honors the
  minimal-deps goal.
- **All UI access in scenes goes through our own thin `ui::` facade** (e.g.
  `ui::SliderFloat(...)`). If hand-rolling the UI ever becomes a drag on momentum, we
  can back that facade with **Dear ImGui** (vendored, single integration point) by
  changing one file — *scene code never changes*. This keeps the option open without
  committing to it now.

This is the only sanctioned dependency expansion, and it's deliberately quarantined.

---

## 5. Repository layout

```
codephys/
├─ CMakeLists.txt              # top-level; orchestrates subdirs
├─ CMakePresets.json           # windows-msvc / linux-gcc / linux-clang presets
├─ README.md                   # build & run instructions
├─ docs/
│  ├─ design/DESIGN.md         # this file
│  └─ Textbooks/               # College Physics 2e (reference, gitignored if large)
├─ third_party/
│  ├─ glfw/                    # submodule or fetched via CMake FetchContent
│  ├─ glad/                    # vendored generated GL loader (src + include)
│  └─ doctest/                 # (optional) single-header test framework
├─ physics/                    # THE CUSTOM PHYSICS LIBRARY (no rendering deps)
│  ├─ include/physics/
│  │  ├─ math/                 # vec, mat, quaternion, constants, units
│  │  ├─ core/                 # State, Integrator, World, Body, Particle
│  │  ├─ forces/               # gravity, spring, drag, electrostatic, ...
│  │  ├─ constraints/          # collision, contact, distance, pin
│  │  └─ fields/               # scalar/vector field sampling (E, B, g, potential)
│  └─ src/
├─ render/                     # hybrid 2D/3D renderer over OpenGL
│  ├─ include/render/
│  └─ src/
├─ ui/                         # hand-rolled immediate-mode UI (facade in ui::)
├─ platform/                   # GLFW window, input, GL context, main loop, timing
├─ app/                        # the playground executable
│  ├─ main.cpp                 # creates window, runs SceneManager (gallery)
│  ├─ scene.hpp                # Scene interface
│  └─ scenes/                  # one file (or folder) per simulation
│     ├─ ch02_projectile.cpp
│     ├─ ch07_energy_skatepark.cpp
│     └─ ...
└─ tests/                      # analytic-validation + unit tests (headless)
```

**Dependency direction is strict and one-way:**

```
app  ──▶ scenes ──▶ { physics, render, ui }
render ──▶ physics::math      (shares vector/matrix types only)
ui     ──▶ render             (draws through the renderer)
physics ──▶  (nothing but the standard library)   ← the crown jewel stays pure
platform ──▶ GLFW
```

`physics/` knows nothing about windows, OpenGL, or UI. That isolation is what lets us
test it headlessly and reuse it forever.

---

## 6. Build & run (Windows + Linux/WSL)

One toolchain, two presets. GLFW is pulled with CMake `FetchContent` (pinned tag) so
there's nothing to install by hand beyond a compiler + CMake.

```bash
# Linux / WSL2 (needs: build-essential cmake libgl-dev; WSLg provides the display)
cmake --preset linux-gcc
cmake --build --preset linux-gcc
./build/linux-gcc/app/codephys

# Windows (VS 2022 / MSVC)
cmake --preset windows-msvc
cmake --build --preset windows-msvc --config RelWithDebInfo
.\build\windows-msvc\app\RelWithDebInfo\codephys.exe
```

**WSL note:** WSL2 + WSLg renders GUI apps natively (Win11 has this built in — your
environment qualifies). If GL version issues arise under WSLg's software/virtual GPU,
we fall back to requesting an OpenGL 3.3 *compatibility* hint or `LIBGL_ALWAYS_SOFTWARE`
(documented in README). This is the main cross-platform risk and we test it early
(Phase 0).

---

## 7. Architecture overview

Layered, with the physics core at the bottom and the app at the top. Each layer depends
only downward.

```
┌────────────────────────────────────────────────────────────┐
│  app: SceneManager (gallery) + main loop + time control      │
├───────────────┬──────────────────────────┬──────────────────┤
│   ui (IMGUI)  │   scenes (one per sim)    │  readouts/plots   │
├───────────────┴──────────────────────────┴──────────────────┤
│  render: Renderer2D / Renderer3D, Camera, primitives, text   │
├──────────────────────────────────────────────────────────────┤
│  physics: World, Integrators, Bodies, Forces, Constraints,    │
│           Fields   ── PURE, no rendering, fully testable      │
├──────────────────────────────────────────────────────────────┤
│  physics::math (shared)   │   platform: GLFW window/input/GL  │
└──────────────────────────────────────────────────────────────┘
```

### 7.1 The main loop & time control

A **fixed-timestep** simulation loop with an accumulator (Glenn Fiedler's
"fix your timestep" pattern) so physics is deterministic and frame-rate-independent;
rendering interpolates between the two most recent states for smoothness.

```
accumulator += frameTime (clamped)
while (accumulator >= dt):
    world.previousState = world.state
    world.step(dt)                  # one physics tick
    accumulator -= dt
alpha = accumulator / dt
render( lerp(previousState, state, alpha) )
```

Time control exposed in the UI: **Play / Pause / Step-one-tick / Reset**, plus a
**time-scale** slider (slow-mo to fast-forward) and a **dt** selector — because letting
the learner crank `dt` up and watch a stable scheme blow up *is the lesson* on numerical
stability.

### 7.2 Renderer (hybrid 2D/3D)

A scene declares its dimension; the renderer offers a unified immediate-style API:

```cpp
struct Camera { /* 2D ortho OR 3D perspective+orbit; mode is a field */ };

class Renderer {
public:
  void begin(const Camera&);                 // sets view/projection
  // 2D primitives (z ignored / fixed)
  void line2D(vec2 a, vec2 b, Color, float width=1);
  void circle(vec2 c, float r, Color, bool filled=true);
  void polygon(span<vec2>, Color, bool filled=true);
  void arrow2D(vec2 from, vec2 to, Color);   // for force/velocity vectors
  // 3D primitives
  void line3D(vec3 a, vec3 b, Color);
  void mesh(const Mesh&, const mat4& model, Color);   // sphere/box/plane prebuilt
  void grid3D(...); void arrow3D(...);
  // shared
  void text(vec2 screenPos, std::string_view, Color);
  void end();                                 // flush batched geometry
};
```

Internals: a handful of small GLSL shaders (flat-color, simple Lambert for 3D), **batched**
line/triangle buffers, and **instanced** draws for repeated primitives (e.g. thousands of
field arrows or N particles). Cameras: `Camera2D` (pan/zoom ortho) and `Camera3D` (orbit/
pan/dolly around a target). Picking the dimension is per-scene; the gallery sets it up
from `Scene::dimension()`.

### 7.3 UI (immediate-mode, `ui::` facade)

Immediate-mode so scene code reads like:

```cpp
void MyScene::ui() {
  ui::SliderFloat("Gravity (m/s^2)", &g, 0.f, 25.f);
  ui::SliderFloat("Launch angle (deg)", &angleDeg, 0.f, 90.f);
  if (ui::Button("Fire")) reset();
  ui::Checkbox("Show velocity vector", &showV);
  ui::PlotLine("Energy (J)", energyHistory);
  ui::LabelEquation("R = v0^2 sin(2θ)/g");   // governing eqn, from the textbook
}
```

Implemented as a tiny retained-free widget set drawing through `Renderer` + handling
GLFW input state. (Backed by Dear ImGui later if needed — see §4.)

---

## 8. The custom physics library (the heart)

`physics/` is engine-agnostic, header-light, and **the part we most want to get right**.
Designed bottom-up.

### 8.1 Math (`physics/math`)
- `vec2`, `vec3`, `vec4`, `mat3`, `mat4`, `quat` — `constexpr`, value types, no SIMD
  intrinsics in v1 (clarity over speed; the math is the learning material).
- `constants.hpp`: `G` (gravitation), `g_earth`, `c`, `k_e` (Coulomb), `eps0`, `mu0`,
  `h`, `k_B`, … all in **SI**, sourced and cross-checked against the textbook's units
  chapter (§1.2).
- **Units discipline:** everything is SI internally; UI does display-unit conversion
  only. We don't build a full compile-time dimensional-analysis system in v1 (tempting,
  but scope) — instead we name variables with units (`v0_mps`, `angle_rad`) and
  centralize conversions in one header.

### 8.2 State & integrators (`physics/core`)
The pedagogical centerpiece. A body's motion is `dy/dt = f(y, t)`; we expose multiple
integrators behind one interface so scenes can **switch at runtime**:

```cpp
enum class Integrator { ExplicitEuler, SemiImplicitEuler, Verlet, RK4 };
```

- **Explicit (Forward) Euler** — simplest, visibly gains energy. Teaching baseline.
- **Semi-implicit (symplectic) Euler** — cheap, energy-stable for oscillators/orbits.
- **Velocity Verlet** — great for gravitation/molecular-style sims; time-reversible.
- **RK4** — high accuracy per step; the "gold standard" for comparison.

Each scene can display *the same initial conditions under each integrator side by side*
— a direct, visual answer to "why does my orbit spiral out?"

### 8.3 Bodies & the World
- `Particle` — point mass: position, velocity, mass, accumulated force. (Covers Ch 2–8,
  much of E&M as test charges.)
- `RigidBody2D` — adds orientation, angular velocity, moment of inertia, torque
  accumulator. (Ch 9–10 statics/rotation.)
- `RigidBody3D` — quaternion orientation + inertia tensor (later phase).
- `World` — owns bodies, a list of **force generators**, and **constraints**; `step(dt)`
  runs: clear accumulators → apply forces → integrate → resolve constraints → record
  conserved quantities.

### 8.4 Forces (`physics/forces`) — composable `ForceGenerator`s
`Gravity (uniform)`, `NewtonianGravity (inverse-square, N-body)`, `Spring (Hooke)`,
`Damping/Drag (linear & quadratic)`, `Buoyancy`, `ElectrostaticCoulomb`,
`LorentzForce (qv×B)`, `Friction (static/kinetic)`. Each is a small class implementing
`apply(World&, dt)`. New physics = new force generator; this composability is what makes
the library reusable across chapters.

### 8.5 Constraints & collisions (`physics/constraints`)
- Distance/pin constraints (pendulums, rigid rods).
- 2D collision: circle-circle, circle-line, polygon (SAT) with impulse-based resolution
  and a **restitution coefficient** (elastic ↔ inelastic — directly Ch 8). Naive O(N²)
  broad phase is fine for our body counts.

### 8.6 Fields (`physics/fields`)
Sampleable scalar/vector fields for visualization: gravitational `g(r)`, electric
`E(r)` & potential `V(r)` from charge distributions, magnetic `B(r)`. The renderer draws
these as field-line / arrow-grid / heatmap overlays (Ch 18–24).

### 8.7 Validation hooks
Every integrator step can emit `(KE, PE, p, L)`. Tests assert these against analytic
expectations (energy conserved by symplectic schemes within tolerance, momentum
conserved in collisions, etc.) — see §10.

---

## 9. Curriculum → simulation mapping

The 34 textbook chapters, triaged into **Dynamical sim** (full interactive physics),
**Field/visualization** (compute & render fields/rays, limited dynamics), and
**Explainer/visualization** (modern physics — animated concept, not an ODE solve).

| Ch | Topic | Treatment | Flagship scene(s) |
|----|-------|-----------|-------------------|
| 1 | Units, measurement | — | (foundational: math/units module + a "unit explorer") |
| 2 | 1D Kinematics | Dynamical | Free-fall & v-t/x-t graph plotter |
| 3 | 2D Kinematics | Dynamical | **Projectile** w/ range eqn, drag toggle |
| 4 | Newton's Laws | Dynamical | Forces on a block, free-body diagram overlay |
| 5 | Friction, drag, elasticity | Dynamical | Inclined plane (static→kinetic), terminal velocity |
| 6 | Circular motion & gravitation | Dynamical | **Orbit sandbox** (N-body, integrator compare) |
| 7 | Work & energy | Dynamical | **Energy skatepark** (KE/PE/total bars) |
| 8 | Momentum & collisions | Dynamical | Elastic/inelastic collision lab (restitution slider) |
| 9 | Statics & torque | Dynamical | Balance beam / bridge truss, see-saw |
| 10 | Rotation & angular momentum | Dynamical | Spinning bodies, conservation of L (figure skater) |
| 11 | Fluid statics | Field/vis | Buoyancy & pressure-vs-depth |
| 12 | Fluid dynamics | Field/vis | Streamlines / continuity & Bernoulli (later phase) |
| 13 | Temp, kinetic theory, gas laws | Dynamical | **Ideal-gas particle box** (P-V-T, Maxwell-Boltzmann histogram) |
| 14 | Heat & transfer | Field/vis | 1D/2D heat-diffusion grid |
| 15 | Thermodynamics | Field/vis | P-V diagram engine cycles |
| 16 | Oscillations & waves | Dynamical | **SHM** (mass-spring, pendulum), wave-on-string |
| 17 | Sound/hearing | Field/vis | Wave superposition, beats, Doppler |
| 18 | Electric charge & field | Field/vis | **Charge sandbox** → E-field lines & equipotentials |
| 19 | Electric potential | Field/vis | Potential heatmap, test-charge motion |
| 20 | Current, resistance, Ohm | Dynamical | Circuit sim (resistors), V/I readouts |
| 21 | DC circuits | Dynamical | RC charge/discharge, Kirchhoff networks |
| 22 | Magnetism | Field/vis | B-field viz, charged particle in B (cyclotron) |
| 23 | Induction, AC | Field/vis | Faraday loop, RLC response |
| 24 | EM waves | Explainer/vis | Propagating E×B animation |
| 25 | Geometric optics | Field/vis | **Ray tracer** (lenses/mirrors), Snell's law |
| 26 | Vision & instruments | Field/vis | Eye/lens model, microscope/telescope ray diagram |
| 27 | Wave optics | Field/vis | Double-slit interference, diffraction patterns |
| 28 | Special relativity | Explainer/vis | Time dilation / length contraction visualizer |
| 29–34 | Quantum → Frontiers | Explainer/vis | Curated concept animations (photoelectric, Bohr orbits, decay, etc.) |

This mapping is the backbone of the roadmap. We don't build it top-to-bottom; we build
the **engine capabilities** each cluster needs (§10).

---

## 10. Roadmap & milestones

Phased so that **every phase ends with something runnable and visibly cool**, and each
phase adds a reusable engine capability rather than a one-off.

### Phase 0 — Skeleton & "hello triangle" (cross-platform proof)
- CMake + presets; GLFW via FetchContent; glad vendored.
- Open a window on **both** Windows and WSL; clear screen; draw a triangle; render text.
- **Exit criterion:** identical build/run on Win + WSL. (De-risks the #1 unknown early.)

### Phase 1 — Math + core loop + 2D renderer + minimal UI
- `physics::math` with unit tests.
- Fixed-timestep loop with interpolation; Play/Pause/Step/Reset/time-scale.
- `Renderer` 2D primitives (lines, circles, polygons, arrows, text) + `Camera2D`.
- `ui::` slider/button/checkbox/label/plot.
- `Particle`, `World`, all four integrators.
- **Flagship deliverable: Projectile scene (Ch 3)** + **integrator-comparison demo**
  (Euler vs RK4 on a circular orbit, side by side).

### Phase 2 — Forces, energy, collisions (classical mechanics core)
- Force generators: uniform & Newtonian gravity, spring, drag, friction.
- 2D collisions + restitution; distance/pin constraints.
- Conserved-quantity readouts + phase-space plot.
- **Scenes:** Orbit sandbox (Ch 6), Energy skatepark (Ch 7), Collision lab (Ch 8),
  SHM mass-spring & pendulum (Ch 16), inclined plane (Ch 5).

### Phase 3 — Rotation, statics, gases
- `RigidBody2D` (torque, inertia, angular momentum).
- **Scenes:** balance beam/torque (Ch 9), angular-momentum conservation (Ch 10),
  ideal-gas particle box with Maxwell-Boltzmann histogram (Ch 13).

### Phase 4 — Hybrid 3D
- `Renderer3D` (perspective/orbit camera, Lambert-lit meshes, instancing), `RigidBody3D`.
- **Scenes:** 3D orbital mechanics, gyroscope/precession.

### Phase 5 — Fields & E&M
- `physics::fields` + field-line/heatmap/arrow-grid rendering.
- **Scenes:** charge sandbox + E-field/equipotentials (Ch 18–19), Lorentz force /
  cyclotron (Ch 22), RC & RLC circuits (Ch 21–23).

### Phase 6 — Waves & optics
- Wave-on-string & superposition (Ch 16–17), ray tracer for lenses/mirrors (Ch 25–26),
  double-slit interference (Ch 27).

### Phase 7 — Modern-physics explainers
- Curated visualizations: relativity (Ch 28), photoelectric effect & Bohr model
  (Ch 29–30), radioactive decay (Ch 31). These are animated concept scenes, not ODE
  solvers.

> Phases 0–2 are the committed near-term plan; 3+ are sequenced but we'll re-plan after
> Phase 2 once the engine's real ergonomics are known.

---

## 11. Testing & correctness strategy

The whole reason to write our own physics is to trust it — so we prove it.

- **Analytic validation** (the gold standard): assert sims match closed-form solutions.
  - Projectile range `R = v₀²·sin(2θ)/g`; max height; time of flight.
  - SHM period `T = 2π√(m/k)`; small-angle pendulum `T = 2π√(L/g)`.
  - Circular-orbit period vs Kepler's third law.
  - Elastic collision: KE and momentum conserved; 1D two-body final velocities.
- **Invariants:** symplectic integrators keep total energy within tolerance over N
  periods; momentum conserved in isolated systems; angular momentum conserved with no
  external torque.
- **Numerical convergence:** error scales with `dt` at each integrator's expected order
  (Euler O(dt), RK4 O(dt⁴)) — a test that *also documents* the math.
- **Unit tests** for `physics::math` (dot/cross/normalize/matrix-inverse/quaternion).
- **Headless:** all physics tests run with no window, in CI, on both compilers.
- **CI:** GitHub Actions matrix (windows-msvc, ubuntu-gcc, ubuntu-clang) — configure,
  build warnings-as-errors, run tests.

---

## 12. Conventions

- **C++20**, `snake_case` for files, `PascalCase` types, `camelCase` members/functions
  (we'll lock this in a `.clang-format` + `.clang-tidy` in Phase 0).
- SI units everywhere internally; unit suffix in variable names at boundaries.
- `physics/` has **zero** non-stdlib includes. Enforced by review (and ideally a CMake
  link-isolation check).
- Doc comments cite the textbook section (`// College Physics 2e §7.2`) next to the
  equation they implement — so the code is a study companion.

---

## 13. Risks & open questions

| Risk | Mitigation |
|---|---|
| OpenGL under WSLg flaky / wrong GL version | ✅ **Resolved in Phase 0.** Verified on both targets — Windows (MSVC): GL 3.3.0 hardware, Intel Arc Pro Graphics, GLSL 3.30. Linux/WSL2 (GCC): GL 4.5 Core, Mesa 25.2.8 `llvmpipe` (software), GLSL 4.50 — both render the triangle with no GL errors. WSLg defaults to software `llvmpipe` (≥ GL 4.5, well past our 3.3 floor); `LIBGL_ALWAYS_SOFTWARE=1` behaves identically. GLFW is built X11-only (no Wayland dep). Fallback documented in README. |
| Hand-rolled UI becomes a time sink | `ui::` facade lets us drop in Dear ImGui at one seam (§4) |
| Scope creep across 34 chapters | Triage (§9) + phase gates (§10); modern physics = explainers |
| Hand-rolled text rendering fiddly | Start with a simple embedded bitmap font; SDF later if wanted |
| Collision math (SAT/impulses) is subtle | Restrict v1 to circles + lines + convex polys; analytic tests |

**Resolved decisions:**
1. ✅ **Repo hosting / CI** — **GitHub + Actions.** Repo initialized with git (`main`
   branch); `.gitignore` added (build artifacts, IDE folders, and the 251 MB textbook
   PDF are excluded). The CI matrix (§11) lands with the Phase 0 code so it has something
   to build (no red builds on an empty repo).
3. ✅ **Test framework** — **vendored single-header `doctest`** under `third_party/`.
   Accepted as a "near-zero dependency" compromise: it's header-only, build-time only,
   ships nothing into the app binary, and keeps the runtime dependency count at exactly
   one (GLFW).
4. ✅ **First scene** — **projectile (Ch 3)** is the "hello, physics" milestone (Phase 1).

**Still open:**
2. **License** — public/open-source? (The textbook is CC BY-NC-SA; our *code* can be any
   license — just don't redistribute the PDF, which `.gitignore` now enforces.) Leaning
   MIT for the code; confirm when convenient.

**Workflow:** development is driven through **OpenSpec** (spec-driven). Change proposals,
specs, and tasks live in `openspec/`; see `AGENTS.md` / `.claude/` for the agent
conventions OpenSpec generated. Phase 0 will be the first OpenSpec change proposal.

---

## 14. Appendix — references
- OpenStax _College Physics 2e_ — `docs/Textbooks/college-physics-2e_-_WEB.pdf`
  (34 chapters; see §9 mapping). CC BY-NC-SA 4.0.
- Glenn Fiedler, "Fix Your Timestep!" — the fixed-dt + accumulator loop (§7.1).
- Standard references we'll lean on for numerics: symplectic integrators, velocity
  Verlet, RK4.
