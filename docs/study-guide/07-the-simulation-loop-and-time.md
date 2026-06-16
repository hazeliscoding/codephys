# 07 · The Simulation Loop & Time

[← Gravitation & Orbits](06-gravitation-and-orbits.md) · [Index](README.md) · [Next: Architecture, Rendering & UI →](08-architecture-rendering-and-ui.md)

---

## 🎯 Learning objectives

- Explain why physics is stepped at a **fixed timestep** while rendering runs at a variable rate.
- Describe the **accumulator** pattern and **state interpolation**.
- Define **determinism** and why it matters for a teaching engine.
- Map every **time-control** button to what it does to the loop.

## 📖 Textbook map

Not a physics chapter — this is the simulation scaffolding. Reference: Glenn Fiedler,
_"Fix Your Timestep!"_ The physics it carries is everything in [§02](02-kinematics-and-projectile-motion.md)–[§06](06-gravitation-and-orbits.md).

---

## 🧠 Concepts & equations

### The problem

Real time arrives in irregular chunks (a frame might take 16 ms, then 20 ms, then 9 ms). But
the integrators in [§05](05-numerical-integration.md) behave best — and **deterministically**
— at a *fixed* $\Delta t$. If you stepped physics by the wildly varying frame time, results
would change with frame rate and even with the same inputs run twice.

### The fix: fixed-`dt` accumulator

Decouple "how often we draw" from "how big a physics step is." Keep a running
**accumulator** of un-simulated real time; spend it in fixed `dt` chunks:

```text
accumulator += clamp(frameTime) * timeScale
while accumulator >= dt:
    world.step(dt)          # always the same dt
    accumulator -= dt
alpha = accumulator / dt    # leftover fraction, in [0, 1)
render(interpolate(previous, current, alpha))
```

- **Clamping** `frameTime` (e.g. to 0.25 s) prevents the *spiral of death*: if one frame
  stalls, we don't try to simulate a huge backlog all at once.
- **`alpha`** is the leftover fraction of a step. Rendering the state interpolated by `alpha`
  removes visual stutter when the draw rate and `dt` don't divide evenly.

### Determinism

Same initial conditions + same `dt` + same integrator ⇒ **identical trajectory, every run**.
This is what makes the engine trustworthy: a test can assert an exact outcome, and a learner's
experiment is reproducible. (It's why `dt` is fixed and the math is `double`.)

---

## 💻 In the code

The loop lives in [`app/main.cpp`](../../app/main.cpp), driven by the per-frame callback from
[`platform::Window::run`](../../platform/src/window.cpp):

```cpp
const double now   = platform::now_seconds();
const double frame = std::min(now - last_time, 0.25);   // clamp catch-up spiral
last_time = now;

double alpha = 0.0;
if (playing) {
    accumulator += frame * time_scale;                  // time-scale stretches/squeezes time
    while (accumulator >= dt) {
        scene->update(dt);                              // one fixed physics tick
        accumulator -= dt;
    }
    alpha = accumulator / dt;                           // for render interpolation
} else if (step_request) {
    scene->update(dt);                                  // exactly one tick while paused
    step_request = false;
}
```

Scenes interpolate their drawn state with `alpha` (the projectile lerps between its previous
and current position), so motion looks smooth even though physics moves in discrete hops.

### Time controls → loop effects

| Control | Effect on the loop |
|---------|--------------------|
| **Play / Pause** (or `Space`) | gates whether the accumulator advances |
| **Step (1 frame)** | while paused, runs exactly one `scene->update(dt)` |
| **Reset** | calls `scene->reset()` — restore initial conditions |
| **time scale** | multiplies real time → more/fewer ticks per second (same `dt`) |
| **dt (s)** | size of each physics step — the stability knob from [§05](05-numerical-integration.md) |

> [!NOTE]
> **time scale vs dt** are different knobs. *time scale* changes how many ticks run per real
> second (fast-forward / slow-mo) without touching accuracy. *dt* changes the size of each
> step, which directly changes accuracy and stability.

### Determinism is tested

[`tests/test_physics.cpp`](../../tests/test_physics.cpp):

```cpp
const Particle a = run();
const Particle b = run();
CHECK(a.position == b.position);   // bitwise-identical trajectories
CHECK(a.velocity == b.velocity);
```

---

## 🔬 Try it

1. **Pause + Step.** Pause, then tap **Step** — the world advances one `dt` per tap. Great for
   inspecting a trajectory frame by frame.
2. **Slow-mo / fast-forward.** Slide **time scale** down to crawl, up to blur. The *shape* of
   the motion is unchanged — only the playback rate.
3. **Stability via dt.** Slide **dt** up with Euler selected and watch accuracy degrade; this
   is the same knob, isolated from time scale.

---

## ✅ Check yourself

<details>
<summary>1. Why not just step physics by the real frame time each frame?</summary>

Because results would depend on frame rate and wouldn't be reproducible. Integrator accuracy
and stability assume a consistent `dt`; a varying step makes the sim non-deterministic and
frame-rate-dependent.
</details>

<details>
<summary>2. What is the "spiral of death" and how is it avoided?</summary>

If a frame takes very long, the accumulator demands many catch-up steps, which take even
longer, demanding still more — a runaway. **Clamping** the frame time (here to 0.25 s) caps
the backlog so the sim slows down gracefully instead of freezing.
</details>

<details>
<summary>3. What does <code>alpha</code> do, and what range is it in?</summary>

`alpha = accumulator / dt` ∈ [0, 1) is the leftover fraction of a step. Rendering the state
**interpolated** by `alpha` between the previous and current physics states removes stutter
when the draw rate doesn't divide evenly into `dt`.
</details>

<details>
<summary>4. Difference between raising "time scale" and raising "dt"?</summary>

**time scale** = more ticks per real second (same step size, same accuracy) — playback speed.
**dt** = bigger individual steps (fewer, coarser) — changes accuracy/stability. One is a clock,
the other is a microscope.
</details>

---

[← Gravitation & Orbits](06-gravitation-and-orbits.md) · [Index](README.md) · [Next: Architecture, Rendering & UI →](08-architecture-rendering-and-ui.md)
