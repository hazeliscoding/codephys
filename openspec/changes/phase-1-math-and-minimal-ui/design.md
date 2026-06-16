## Context

Phase 0 left a wired-but-empty skeleton: module targets with enforced layering, a GLFW +
OpenGL 3.3 context, a headless doctest harness, and CI on three legs. `physics::math` is a
single placeholder `Vec2`; `app/main.cpp` draws a throwaway triangle; `render`/`ui` are
stubs. Phase 1 fills these in to produce the first genuinely useful build: a deterministic
simulation core, a 2D renderer, an immediate-mode UI, and the flagship Projectile scene
validated against its closed-form solution.

This is cross-cutting (every first-party module gains real code, and the full
`app → scenes → {physics, render, ui}` graph runs for the first time) and introduces the
engine's core abstractions — integrators, the step pipeline, the renderer API, the UI
model — so the decisions below are worth settling before coding. The dominant constraints
are the project hard rules: **`physics/` stays stdlib-only**, **GLFW is the only runtime
dep**, **SI units internally**, and **cross-platform parity (MSVC / WSLg)**.

## Goals / Non-Goals

**Goals:**
- A `physics::math` (constexpr value types, SI constants, unit conversions) that both
  `physics` and `render` share via the `physics::math` edge — with no rendering leaking in.
- A `physics::core` where `Particle` + `World` step deterministically under **four
  runtime-switchable integrators** behind one interface, emitting conserved quantities.
- A fixed-timestep loop with render interpolation and full time control
  (play/pause/step/reset/time-scale/dt).
- A renderer-agnostic 2D `Renderer` (lines/circles/polygons/arrows/text + `Camera2D`) and
  an immediate-mode `ui::` widget set drawing through it.
- A `Scene` interface + gallery, the Projectile scene (Ch 3), and an integrator-comparison
  demo — all validated by headless analytic/invariant/convergence tests in CI.

**Non-Goals:**
- Force generators beyond uniform gravity, collisions, constraints, `RigidBody2D`/rotation,
  3D, fields/E&M (Phases 2–5). The orbit demo uses an inline inverse-square acceleration,
  not the general force system.
- SIMD/performance work; a compile-time units system; Dear ImGui; networked or persisted state.

## Decisions

### D1 — `physics::math`: constexpr value types + free functions, double precision
Plain aggregate structs (`Vec2/3/4`, `Mat3/4`, `Quat`) with `constexpr` operators and free
functions (`dot`, `cross`, `length`, `normalize`, `inverse`, …). **`double`** is the
physics scalar (accuracy and stable cross-compiler analytic tests matter more than speed;
clarity over SIMD). A `constants.hpp` holds SI constants cited to the textbook (§1.2), and a
`units.hpp` centralizes display conversions (deg↔rad, etc.). The math headers become the
real `physics::math` target surface that `render` consumes (Phase 0 already wired the
edge/alias); `render` uses these types for camera/vertex math, never the reverse.
- *Alternatives:* a class with private members (more boilerplate, no aggregate init);
  `float` (faster, but noisier convergence tests and worse orbit energy drift); templated
  scalar `Vec<T,N>` (premature generality — the math is learning material, keep it explicit).

### D2 — Integrators behind an acceleration-function interface
Motion is `dy/dt = f(y,t)` with `y = (position, velocity)`. Rather than couple integrators
to the full `World`, each integrator advances a particle's `(x, v)` given an
**acceleration functor** `a(x, v, t)`. This is the one abstraction that makes all four
schemes fit one interface and lets RK4/Verlet sample acceleration mid-step:
```cpp
enum class Integrator { ExplicitEuler, SemiImplicitEuler, Verlet, RK4 };
// integrate(state, accel_fn, t, dt) -> new state
```
- Explicit Euler (visibly gains energy — the teaching baseline), Semi-implicit Euler
  (symplectic, energy-stable), Velocity Verlet (time-reversible), RK4 (gold standard).
- *Alternatives:* integrators that take `World&` directly (couples them to force iteration,
  harder to unit-test in isolation); a generic ODE state vector `std::vector<double>` (loses
  the legible `(x,v)` shape that is itself the lesson). The functor keeps each scheme a small,
  separately testable function.

### D3 — `World::step(dt)` pipeline; Phase 1 force model = acceleration-from-state
`World` owns `Particle`s and runs: **clear force accumulators → compute acceleration →
integrate with the selected scheme → record conserved quantities (KE, PE, p, L)**. Phase 1
has no `ForceGenerator` system yet, so acceleration comes from a per-world acceleration
function (uniform gravity `a = g` for projectile; inline inverse-square `a = -μ r̂ / |r|²`
for the orbit demo). The accumulator field and pipeline shape are designed so Phase 2 drops
in `ForceGenerator`s without reshaping `step`.
- *Trade-off:* introducing the accumulator now without using it fully is mild over-design,
  but it keeps the `step` contract stable across the Phase 1→2 boundary.

### D4 — Fixed-timestep loop with interpolation, owned by `app`
The accumulator loop (DESIGN §7.1) lives in `app` (it orchestrates physics + render + ui):
```
accumulator += clamp(frameTime) * timeScale
while accumulator >= dt: world.prev = world.state; world.step(dt); accumulator -= dt
render(lerp(prev, state, accumulator/dt))
```
Time control is explicit state in `app`: **Play/Pause** (gate the accumulator), **Step**
(advance exactly one `dt` while paused), **Reset** (re-init the active scene), a
**time-scale** slider, and a **dt selector** (so a learner can crank `dt` and watch a
scheme blow up). `frameTime` is clamped to avoid the spiral-of-death.
- *Alternatives:* variable timestep (non-deterministic, defeats the integrator lesson);
  fixed dt with no interpolation (visible stutter at high time-scale). Fixed + interpolation
  is the design's stated approach.

### D5 — Renderer: two shaders, one batched dynamic buffer, ortho `Camera2D`
A single `Renderer` batches colored vertices into a dynamic VBO flushed on `end()`, using
two small GLSL 3.30 programs (flat-colored triangles; flat-colored lines). Primitives reduce
to triangles/lines: `circle`/`polygon` = triangulated fans, `line2D` with width = a quad,
`arrow2D` = shaft quad + head triangle. `Camera2D` is a pan/zoom orthographic view producing
a view-projection `Mat3`/`Mat4`; world↔screen helpers support picking and UI. **Text** uses
an **embedded public-domain 8×8 bitmap font** uploaded once as an `R8` atlas texture; glyphs
are textured quads in a third tiny program (or the triangle program with a texture flag).
All GL lives in `render`/`platform`; the renderer API is GL-agnostic to callers.
- *Alternatives:* one shader with branching (fine, but two is clearer); SDF/TTF text (Phase
  0 explicitly deferred typography; a bitmap font is the minimal proof and is plenty for
  readouts); per-primitive draw calls (too many; batch instead). Instancing is deferred to
  when a scene needs thousands of primitives.

### D6 — Immediate-mode UI with an injected input snapshot (no `ui → platform` dep)
`ui::` is retained-free: each frame the widgets are re-declared, lay out via a simple cursor,
and draw through `Renderer`. Hot/active-item tracking uses IDs derived from call order/labels.
To respect layering (`ui → render` only, never `ui → platform/GLFW`), `app` reads GLFW input
through `platform` and passes a plain **`InputState` snapshot** (mouse pos, button states,
pressed keys, scroll) into the UI/scene each frame. Text metrics come from the renderer's
font.
- *Alternatives:* `ui` calling GLFW directly (breaks layering); a retained widget tree
  (heavier, and the design wants the immediate-mode authoring ergonomics in §7.3). Passing an
  input snapshot keeps `ui` pure and testable.

### D7 — `Scene` interface + `SceneManager` gallery
```cpp
class Scene {
  virtual void reset() = 0;
  virtual void update(double dt) = 0;          // advance physics
  virtual void render(Renderer&) = 0;
  virtual void ui(const InputState&) = 0;      // control panel
  virtual Dimension dimension() const = 0;     // 2D in Phase 1
  virtual std::string_view name() const = 0;
};
```
`SceneManager` owns the registered scenes, drives the selected one through the loop, and
shows a gallery list to switch scenes. Phase 1 registers the **Projectile** scene and the
**integrator-comparison** scene (Euler vs RK4 on a circular orbit, drawn side by side).
- *Alternatives:* hard-coding one scene in `main` (no gallery, doesn't exercise the
  framework the rest of the roadmap needs). The interface is intentionally small.

### D8 — Validation: analytic + invariant + convergence, all headless
`physics` tests link doctest + physics only (no GL), per Phase 0's headless rule:
- **Analytic:** projectile range `R = v₀² sin(2θ)/g`, max height, time of flight; circular-
  orbit period vs Kepler.
- **Invariant:** symplectic schemes (semi-implicit Euler, Verlet) keep total energy within
  tolerance over many periods; momentum/angular momentum conserved with no external torque.
- **Convergence:** global error scales at each scheme's order — Euler O(dt), RK4 O(dt⁴) —
  asserted via the error ratio between `dt` and `dt/2`. These tests double as documentation.
- **Math unit tests:** dot/cross/normalize/matrix-inverse/quaternion.

## Risks / Trade-offs

- **Hand-rolled text is fiddly** → Embed a fixed 8×8 bitmap font (no shaping, ASCII only);
  it only needs to render readouts and labels. SDF/TTF deferred.
- **Cross-compiler float divergence breaks tight tests** → Use `double`, no fast-math, and
  tolerance-based asserts; test convergence *order* (error ratios), not absolute values.
- **One integrator interface must fit Verlet (position-based) and RK4 (multi-sample)** →
  The acceleration-functor signature `a(x, v, t)` covers all four; Verlet stores/derives the
  needed prior acceleration internally. Validated by the convergence tests.
- **UI input coupling could leak GLFW into `ui`** → Inject an `InputState` snapshot from
  `app`; `ui` never includes platform/GLFW headers (canary-style: keep the include out).
- **Renderer scope creep** → Phase 1 ships one batched dynamic buffer + ≤3 shaders; no
  instancing/atlasing beyond the font until a scene demands it.
- **WSLg software GL (llvmpipe) performance** → The 2D batched renderer is light; clamp
  `frameTime` so slow frames don't spiral. Parity already proven in Phase 0.

## Migration Plan

Greenfield feature work on top of Phase 0; nothing to roll back at the product level.
Within the repo: the Phase 0 `physics::math::Vec2` placeholder and its uses
(`render/src/render.cpp`, `tests/test_math.cpp`) are replaced by the real math types, and
`app/main.cpp`'s triangle is replaced by the `SceneManager`-driven loop. The `physics::math`
target may be split out from `physics` (its own headers) so `render` links only the math
surface; the existing alias keeps consumers working during the transition. CI stays the
three-leg matrix; the expanded headless tests must remain green on all legs.

## Open Questions

- **`physics::math` target split** — keep the single `physics` target with a `physics::math`
  alias (simplest), or promote `physics_math` to its own header library so `render` links a
  strictly smaller surface? Lean: split when `render` starts including math headers, to keep
  the edge honest. Resolve during implementation.
- **Bitmap font source** — which public-domain 8×8 font to embed (e.g., a classic
  `font8x8`-style table). Any CC0/public-domain ASCII set is fine; record the source like the
  glad README.
- **Integrator comparison as a scene vs a mode** — ship as its own `Scene` (simplest for the
  gallery) vs a toggle inside a generic scene. Lean: its own scene for Phase 1.
