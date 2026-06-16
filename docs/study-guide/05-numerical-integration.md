# 05 · Simulating Motion Faithfully

[← Energy, Momentum & Conservation](04-energy-momentum-and-conservation.md) · [Index](README.md) · [Next: Gravitation & Orbits →](06-gravitation-and-orbits.md)

---

> Real motion is continuous; a computer can only take finite time steps. This section is
> about the **physics consequences** of that — why a simulated pendulum can gain energy and a
> simulated planet can spiral away, and what makes a simulation faithful to the real physics.

## 🎯 Learning objectives

- Explain why stepping time introduces error, and what **energy drift** and **orbital decay**
  look like physically.
- Define what it means for a simulation to **conserve energy** versus merely be **accurate**.
- Predict which stepping method keeps the physics honest for oscillations and orbits.

## 📖 Textbook map

This underpins the mechanics in **Ch. 6–8** (orbits, oscillations, energy). The numerical
detail is methods, not a physics chapter; reference: Glenn Fiedler, _"Integration Basics."_

---

## 🧠 Concepts & equations

### The physical problem

From [§03](03-newtons-laws-and-acceleration.md), motion obeys
$\frac{d\mathbf{v}}{dt} = \mathbf{a}$ and $\frac{d\mathbf{r}}{dt} = \mathbf{v}$. To simulate
it we advance the state in small jumps of size $\Delta t$ (`dt`). Each jump is slightly wrong,
and **how** those small errors accumulate decides whether the simulated physics stays true.

Two very different failure modes matter for physics:

- **Energy drift.** If each step nudges the total energy the same direction, energy grows (or
  shrinks) without limit. A frictionless pendulum should swing forever at constant amplitude —
  a bad method makes it swing *wider and wider*, inventing energy from nothing.
- **Orbital decay / blow-up.** The same error in an orbit shows up as the planet spiraling
  outward (gaining energy) instead of tracing a closed ellipse.

### Faithful ≠ merely accurate

A method can be **accurate** (small error per step) yet still slowly leak energy; or it can be
slightly less accurate yet **conserve energy** beautifully over millions of steps. For
long-running physics — an orbit, an oscillation — *energy faithfulness* often matters more than
raw per-step accuracy.

| Behavior you'll see | What it means physically |
|---------------------|--------------------------|
| Total-energy line creeps **up** over time | the method is inventing energy (not faithful) |
| Energy wiggles but stays **flat** on average | energy is conserved — faithful for oscillations/orbits |
| Orbit **spirals outward** | energy gain, accumulated over each lap |
| Orbit traces the **same circle** | energy and angular momentum conserved |

> [!IMPORTANT]
> The lesson isn't "use the fanciest method." It's that **the way you simulate time is itself
> a physical choice** — it determines whether your simulated world obeys conservation of
> energy. Some simple methods conserve energy on average ("symplectic"); some accurate ones
> don't.

### The four methods, in physics terms

CodePhys lets you switch between four time-stepping methods so you can *watch* the difference:

| Method | Per-step accuracy | Long-run energy behavior |
|--------|:-----------------:|--------------------------|
| **Explicit Euler** | crude | **gains energy** → oscillations grow, orbits spiral out |
| **Semi-implicit Euler** | crude | **bounded energy** → stable orbits/oscillations (cheap & faithful) |
| **Velocity Verlet** | good | **bounded energy**, time-reversible → great for gravity |
| **RK4** | excellent | very accurate; energy drifts only very slowly |

> [!TIP]
> **Halving `dt`** always reduces per-step error (and the more accurate the method, the faster
> that error falls). But for Explicit Euler the energy *still* trends the wrong way — smaller
> steps just delay the spiral. Faithfulness is a property of the *method*, not only the step
> size.

---

## 💻 In the code (brief)

The four methods share one interface, `integrate(method, state, accel, t, dt)`, in
[`physics/core/integrator.cpp`](../../physics/src/integrator.cpp). The only difference between
the two Euler variants is *which velocity advances the position* — and that one line is the
difference between an orbit that decays and one that's stable:

```cpp
// Explicit Euler: position uses the OLD velocity  -> gains energy.
x += v * dt;          v += a * dt;
// Semi-implicit Euler: velocity first, position uses the NEW velocity -> energy stays bounded.
v += a * dt;          x += v * dt;
```

These claims are checked in [`tests/test_physics.cpp`](../../tests/test_physics.cpp): a
harmonic oscillator's energy stays within tolerance for the symplectic methods, while Explicit
Euler's grows past 1.5× — and the convergence order of each method is asserted against `dt`.

---

## 🔬 Try it

The **Integrators (orbit)** scene is built for this ([§06](06-gravitation-and-orbits.md)):
Explicit Euler (red) spirals outward while RK4 (green) holds its circle — same physics, only
the time-stepping differs.

In **Projectile** you can feel it too:

1. Method = **Euler**, drag **`dt`** to its max → the energy plot ramps **up** and the arc
   distorts (energy being invented).
2. Same big `dt`, switch to **Verlet** or **RK4** → energy line flattens, arc snaps clean.
3. **Step** one tick at a time (paused) to compare how far each method moves the ball in a
   single big step.

> [!NOTE]
> Letting you crank `dt` until a "stable" method blows up **is the lesson**, which is why `dt`
> is a front-and-center slider, not a hidden constant.

---

## ✅ Check yourself

<details>
<summary>1. A frictionless pendulum in the sim slowly swings wider and wider. What's happening?</summary>

The integrator is **adding energy** that real physics wouldn't (classic Explicit Euler on
oscillatory motion). Switch to a symplectic method (semi-implicit Euler / Verlet), or reduce
`dt`, to keep the amplitude constant.
</details>

<details>
<summary>2. Is the most accurate method always the best choice for a long-running orbit?</summary>

Not necessarily. A **symplectic** method (semi-implicit Euler, Verlet) keeps energy bounded
forever, so the orbit stays an orbit — even if its per-step accuracy is lower than RK4, which
is more accurate but slowly drifts over very long runs.
</details>

<details>
<summary>3. You shrink <code>dt</code> and Explicit Euler's orbit decays more slowly. Did you fix it?</summary>

No — you only **delayed** it. The energy still trends upward each lap; a smaller step makes the
error per step smaller but doesn't change its one-sided (energy-gaining) nature. Faithfulness
comes from the method, not just the step size.
</details>

<details>
<summary>4. What single change turns energy-gaining Euler into energy-stable Euler?</summary>

Update the **velocity first**, then move the position with that *new* velocity (semi-implicit /
symplectic Euler) instead of moving the position with the old velocity (explicit Euler).
</details>

---

[← Energy, Momentum & Conservation](04-energy-momentum-and-conservation.md) · [Index](README.md) · [Next: Gravitation & Orbits →](06-gravitation-and-orbits.md)
