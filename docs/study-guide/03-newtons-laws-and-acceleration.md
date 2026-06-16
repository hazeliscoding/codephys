# 03 · Newton's Laws & Acceleration

[← Kinematics & Projectile Motion](02-kinematics-and-projectile-motion.md) · [Index](README.md) · [Next: Energy, Momentum & Conservation →](04-energy-momentum-and-conservation.md)

---

## 🎯 Learning objectives

- State **Newton's three laws** and apply $\mathbf{F} = m\mathbf{a}$.
- Express motion as a **first-order ODE system** in $(\text{position}, \text{velocity})$.
- Understand the **acceleration-function** abstraction at the heart of CodePhys.
- Trace one tick of the **`World::step` pipeline**.

## 📖 Textbook map

- _College Physics 2e_ **Ch. 4** — Dynamics: Newton's laws of motion.
- **Ch. 5** — Friction, drag, and applications (forces arrive fully in Phase 2).

---

## 🧠 Concepts & equations

### Newton's three laws

1. **Inertia.** A body keeps its velocity unless a net force acts. (Zero force ⇒ constant velocity.)
2. **$\mathbf{F} = m\mathbf{a}$.** Net force equals mass times acceleration — equivalently
   $\mathbf{a} = \mathbf{F}/m$.
3. **Action–reaction.** Forces come in equal, opposite pairs.

The one we *compute with* is the second law. Rearranged, it says: **if you know the force on
a body, you know its acceleration.**

$$\mathbf{a} = \frac{\mathbf{F}_{\text{net}}}{m}$$

### Motion as an ODE

Acceleration is the second derivative of position. Numerically it's far easier to handle a
*first-order* system, so we split it into two coupled first-order equations using
$\mathbf{y} = (\mathbf{r}, \mathbf{v})$:

$$\frac{d\mathbf{r}}{dt} = \mathbf{v}, \qquad \frac{d\mathbf{v}}{dt} = \mathbf{a}(\mathbf{r}, \mathbf{v}, t) = \frac{\mathbf{F}(\mathbf{r}, \mathbf{v}, t)}{m}$$

This is the single most important idea in the engine: **all of mechanics is "given the
state, compute the acceleration, then step the state forward."** The *how-to-step* part is
[§05](05-numerical-integration.md); the *what's-the-acceleration* part is this section.

---

## 💻 In the code

### The body: `Particle`

A point mass — position, velocity, mass, and a force accumulator (for Phase 2 forces).
[`physics/core/particle.hpp`](../../physics/include/physics/core/particle.hpp):

```cpp
struct Particle {
    Vec2 position;       // m
    Vec2 velocity;       // m/s
    double mass = 1.0;   // kg
    Vec2 force_accum{};  // N, cleared each tick
};
```

### The acceleration function

Instead of hard-wiring a force model, an integrator is handed a **function** that returns
acceleration from the current state. From
[`physics/core/integrator.hpp`](../../physics/include/physics/core/integrator.hpp):

```cpp
// a(position, velocity, t) -> m/s^2
using AccelFn = std::function<Vec2(Vec2 position, Vec2 velocity, double t)>;
```

This one signature covers every Phase 1 scenario:

| Scene | Acceleration function | Physics |
|-------|----------------------|---------|
| Projectile | `a = (0, -g)` | uniform gravity (Newton's 2nd law with $F = mg$) |
| Orbit (integrator demo) | `a = -μ r / |r|³` | inverse-square gravity ([§06](06-gravitation-and-orbits.md)) |

> [!NOTE]
> Because acceleration is `F/m` and our demo forces are proportional to $m$ (gravity), mass
> cancels and the bodies in these scenes all have `mass = 1`. The accumulator and a general
> `ForceGenerator` system (springs, drag, friction) arrive in **Phase 2** — the pipeline is
> already shaped to drop them in without changing `step`.

### One tick: `World::step`

[`physics/core/world.cpp`](../../physics/src/world.cpp) runs the canonical pipeline
(DESIGN §8.3): **clear accumulators → compute acceleration → integrate → record conserved
quantities.**

```cpp
void World::step(double dt) {
    const AccelFn& accel = acceleration_ ? acceleration_ : zero_accel;
    for (Particle& p : particles_) {
        p.force_accum = Vec2{};                          // 1. clear accumulators
        const State next = integrate(integrator_,        // 2+3. compute accel & step
                                     State{p.position, p.velocity}, accel, time_, dt);
        p.position = next.position;
        p.velocity = next.velocity;
    }
    time_ += dt;
    record_conserved();                                  // 4. KE, PE, p, L (see §04)
}
```

The acceleration model and the integration *method* are independent knobs:
`set_acceleration(...)` chooses the physics, `set_integrator(...)` chooses the math.

---

## 🔬 Try it

- In **Projectile**, press **Step (1 frame)** repeatedly while paused. Each press is exactly
  one `World::step(dt)` — watch the ball advance in discrete hops and the velocity arrow
  shorten as it rises (gravity removing $g\cdot dt$ from $v_y$ each tick).
- Set `angle = 90°` (straight up). Horizontal velocity is zero, so the ball rises and falls
  on the same line — Newton's 1st law in $x$ (no force ⇒ no horizontal motion).

---

## ✅ Check yourself

<details>
<summary>1. A 2 kg puck feels a net force of 10 N. What is its acceleration?</summary>

$a = F/m = 10/2 = \mathbf{5}$ m/s². Newton's second law.
</details>

<details>
<summary>2. Why split a second-order ODE into two first-order ones?</summary>

Numerical integrators are written for first-order systems
$\frac{d\mathbf{y}}{dt} = f(\mathbf{y}, t)$. Using $\mathbf{y} = (\mathbf{r}, \mathbf{v})$
turns "position's second derivative is acceleration" into two first-order updates that any
of the four integrators can step uniformly.
</details>

<details>
<summary>3. In the projectile scene, why do all bodies have mass = 1?</summary>

Gravity's force is $F = mg$, so acceleration $a = F/m = g$ is **independent of mass**. The
mass cancels, so it's left at 1. (It will matter once springs/drag — forces not proportional
to $m$ — arrive in Phase 2.)
</details>

<details>
<summary>4. What are the two independent "knobs" on a <code>World</code>, and which is physics vs math?</summary>

`set_acceleration(...)` = the **physics** (what forces act). `set_integrator(...)` = the
**math** (how time is stepped). Keeping them separate is what lets one scene compare
integrators on identical physics.
</details>

---

[← Kinematics & Projectile Motion](02-kinematics-and-projectile-motion.md) · [Index](README.md) · [Next: Energy, Momentum & Conservation →](04-energy-momentum-and-conservation.md)
