# 05 · Numerical Integration

[← Energy, Momentum & Conservation](04-energy-momentum-and-conservation.md) · [Index](README.md) · [Next: Gravitation & Orbits →](06-gravitation-and-orbits.md)

---

> This is the conceptual heart of CodePhys. The same physics, stepped four different ways,
> gives four visibly different answers — and understanding *why* is the point.

## 🎯 Learning objectives

- Explain why we **discretize** continuous motion and what error that introduces.
- Derive and compare **Explicit Euler, Semi-implicit Euler, Velocity Verlet, and RK4**.
- Define **order of accuracy** and predict how error scales with `dt`.
- Explain **symplectic** integration and why it keeps energy bounded.

## 📖 Textbook map

Not an OpenStax chapter — this is the numerical-methods layer underneath all the physics.
References: Glenn Fiedler, _"Integration Basics"_; standard texts on ODE solvers (Euler,
Runge–Kutta, Verlet).

---

## 🧠 Concepts & equations

We want to advance $\mathbf{y} = (\mathbf{x}, \mathbf{v})$ from time $t$ to $t + \Delta t$,
given acceleration $\mathbf{a}(\mathbf{x}, \mathbf{v}, t)$. No method is exact; they differ
in **accuracy** (how fast error shrinks as $\Delta t \to 0$) and **stability** (whether the
error stays bounded over long runs).

### 1. Explicit (Forward) Euler — the naive baseline

Use the *current* values to take one straight step:

$$\mathbf{x}_{n+1} = \mathbf{x}_n + \mathbf{v}_n\,\Delta t, \qquad \mathbf{v}_{n+1} = \mathbf{v}_n + \mathbf{a}_n\,\Delta t$$

Simple, but it **systematically adds energy** to oscillatory/orbital motion → things spiral
outward. First-order accurate.

### 2. Semi-implicit (Symplectic) Euler — one tiny change, huge improvement

Update **velocity first**, then use the *new* velocity for position:

$$\mathbf{v}_{n+1} = \mathbf{v}_n + \mathbf{a}_n\,\Delta t, \qquad \mathbf{x}_{n+1} = \mathbf{x}_n + \mathbf{v}_{n+1}\,\Delta t$$

Still first-order accurate, but **symplectic**: energy error stays *bounded* (oscillates a
little, never drifts away). For the same cost as Euler, orbits stay orbits.

### 3. Velocity Verlet — time-reversible workhorse

$$\mathbf{x}_{n+1} = \mathbf{x}_n + \mathbf{v}_n\,\Delta t + \tfrac{1}{2}\mathbf{a}_n\,\Delta t^2$$
$$\mathbf{v}_{n+1} = \mathbf{v}_n + \tfrac{1}{2}\big(\mathbf{a}_n + \mathbf{a}_{n+1}\big)\,\Delta t$$

Second-order, symplectic, and time-reversible — excellent for gravitation and molecular-style
sims. (Assumes acceleration depends on position, not velocity, which holds for Phase 1.)

### 4. RK4 — the gold standard for accuracy

Sample the derivative four times across the step and take a weighted average:

$$\mathbf{y}_{n+1} = \mathbf{y}_n + \tfrac{\Delta t}{6}\big(\mathbf{k}_1 + 2\mathbf{k}_2 + 2\mathbf{k}_3 + \mathbf{k}_4\big)$$

Fourth-order accurate — error per halving of `dt` drops by ~16×. Not symplectic, so over
*very* long runs energy can slowly drift, but for a given accuracy it's hard to beat.

### Order of accuracy

The **global error** after fixed wall-clock time scales as $\mathcal{O}(\Delta t^{\,p})$:

| Method | Order $p$ | Halving `dt` divides error by | Symplectic? |
|--------|:---------:|:-----------------------------:|:-----------:|
| Explicit Euler | 1 | ~2 | ❌ (gains energy) |
| Semi-implicit Euler | 1 | ~2 | ✅ (bounded energy) |
| Velocity Verlet | 2 | ~4 | ✅ |
| RK4 | 4 | ~16 | ❌ (tiny slow drift) |

> [!IMPORTANT]
> **Accuracy ≠ stability.** Semi-implicit Euler is *less accurate* than RK4 but *more
> faithful to energy* on long orbital runs. Pick the integrator for the job: RK4 when you
> need a precise trajectory, a symplectic method when you need long-term qualitative
> correctness (a stable orbit, a non-decaying pendulum).

---

## 💻 In the code

All four share one interface — `integrate(scheme, state, accel, t, dt)` — in
[`physics/core/integrator.cpp`](../../physics/src/integrator.cpp). The code is a 1:1
transcription of the equations above:

```cpp
// Explicit (Forward) Euler — uses the OLD velocity for position. Gains energy.
State step_explicit_euler(const State& s, const AccelFn& accel, double t, double dt) {
    const Vec2 a = accel(s.position, s.velocity, t);
    return {s.position + s.velocity * dt, s.velocity + a * dt};
}

// Semi-implicit (symplectic) Euler — velocity first, then position with the NEW velocity.
State step_semi_implicit_euler(const State& s, const AccelFn& accel, double t, double dt) {
    const Vec2 a = accel(s.position, s.velocity, t);
    const Vec2 v_new = s.velocity + a * dt;
    return {s.position + v_new * dt, v_new};
}

// Velocity Verlet — half-step using current and next acceleration.
State step_verlet(const State& s, const AccelFn& accel, double t, double dt) {
    const Vec2 a     = accel(s.position, s.velocity, t);
    const Vec2 p_new = s.position + s.velocity * dt + a * (0.5 * dt * dt);
    const Vec2 a_new = accel(p_new, s.velocity, t + dt);
    return {p_new, s.velocity + (a + a_new) * (0.5 * dt)};
}

// RK4 — four samples (k1..k4) averaged 1:2:2:1.
```

Switching method at runtime is just `World::set_integrator(...)`; the UI's **Method** button
cycles through them.

### The order claims are tested

Rather than trust the table, [`tests/test_physics.cpp`](../../tests/test_physics.cpp)
measures the error of a simple-harmonic oscillator at `dt` and `dt/2` and checks the ratio:

```cpp
// Explicit Euler is first order: halving dt halves the error (ratio ~2).
CHECK(e1 / e2 == doctest::Approx(2.0).epsilon(0.2));

// RK4 is fourth order: ratio ~16, well above the lower-order schemes.
CHECK(e1 / e2 > 10.0);
```

…and the symplectic energy bound:

```cpp
CHECK(final_energy(Integrator::SemiImplicitEuler) == doctest::Approx(e0).epsilon(0.05));
CHECK(final_energy(Integrator::ExplicitEuler)     >  e0 * 1.5);  // visibly gains energy
```

---

## 🔬 Try it

The **Integrators (orbit)** scene is built for this ([§06](06-gravitation-and-orbits.md)):
Explicit Euler (red) spirals outward while RK4 (green) holds its circle.

In **Projectile** you can also feel the difference:

1. Method = **Euler**, then drag **`dt` (s)** to its max. The trajectory visibly distorts and
   the energy plot ramps up.
2. Same big `dt`, switch to **Verlet** or **RK4** — the arc snaps back to clean and the energy
   line flattens.
3. **Step** through one tick at a time (paused) and compare how far each method moves the ball
   in a single big step.

> [!NOTE]
> Letting a learner crank `dt` until a "stable" scheme blows up **is the lesson**, which is
> exactly why `dt` is a front-and-center slider rather than a hidden constant.

---

## ✅ Check yourself

<details>
<summary>1. You halve <code>dt</code> and an integrator's error drops to ~1/16. What order is it?</summary>

**Fourth order** ($2^4 = 16$) — that's RK4.
</details>

<details>
<summary>2. Explicit and semi-implicit Euler are both first-order. Why prefer semi-implicit for orbits?</summary>

It's **symplectic**: energy error stays bounded instead of growing without limit. Same cost,
same accuracy order, but orbits don't spiral away. The only change is using the *updated*
velocity to advance position.
</details>

<details>
<summary>3. If RK4 is the most accurate, why keep the others?</summary>

Two reasons: (a) **pedagogy** — seeing Euler fail teaches what "stability" means; (b)
**symplecticity** — for very long runs where you care about qualitative behavior (a planet
that orbits forever), a cheap symplectic method can beat RK4, which slowly drifts.
</details>

<details>
<summary>4. Why does Explicit Euler <em>add</em> energy to an orbit?</summary>

It steps position using the old velocity and never "sees" how the force curves the path
within the step, so it consistently overshoots tangentially — nudging the body to a slightly
larger, faster orbit each lap. The error is biased in one direction (energy gain), so it
accumulates into an outward spiral.
</details>

---

[← Energy, Momentum & Conservation](04-energy-momentum-and-conservation.md) · [Index](README.md) · [Next: Gravitation & Orbits →](06-gravitation-and-orbits.md)
