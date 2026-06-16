# 04 · Energy, Momentum & Conservation

[← Newton's Laws & Acceleration](03-newtons-laws-and-acceleration.md) · [Index](README.md) · [Next: Simulating Motion Faithfully →](05-numerical-integration.md)

---

## 🎯 Learning objectives

- Compute **kinetic** and **potential** energy and the **total mechanical energy**.
- State the **conservation laws** for energy, linear momentum, and angular momentum, and the
  conditions under which each holds.
- Read the engine's per-tick **conserved-quantity report** and use it to judge a simulation's
  health.

## 📖 Textbook map

- _College Physics 2e_ **Ch. 7** — Work and energy; conservation of energy.
- **Ch. 8** — Linear momentum and collisions.
- **Ch. 10.5** — Angular momentum and its conservation.

---

## 🧠 Concepts & equations

### Kinetic & potential energy

**Kinetic energy** — energy of motion:

$$K = \tfrac{1}{2} m v^2 = \tfrac{1}{2} m\,(\mathbf{v}\cdot\mathbf{v})$$

**Gravitational potential energy** near Earth — energy of position:

$$U = m g h$$

**Total mechanical energy** $E = K + U$. With no friction/drag, $E$ is **conserved**: as a
projectile rises it trades $K$ for $U$ and back, keeping the sum constant.

### Linear momentum

$$\mathbf{p} = m\mathbf{v}$$

**Conservation:** the total momentum of a system is constant **if no net external force
acts**. (This is Newton's 2nd law restated: $\mathbf{F} = d\mathbf{p}/dt$.)

### Angular momentum (2D)

About the origin, with the 2D cross from [§01](01-vectors-units-and-si.md):

$$L = m\,(\mathbf{r}\times\mathbf{v}) = m\,(x v_y - y v_x)$$

**Conservation:** $L$ is constant if there is **no net torque**. A *central* force (always
pointing toward one point, like gravity in an orbit) exerts no torque about that point, so
orbits conserve angular momentum — that's Kepler's "equal areas in equal times."

> [!TIP]
> Conservation laws are the engine's **lie detector**. If energy or momentum drifts when it
> shouldn't, your integrator (or your physics) is wrong. CodePhys leans on this hard.

---

## 💻 In the code

Every `World::step` ends by recording these quantities for readouts and tests.
[`physics/core/world.hpp`](../../physics/include/physics/core/world.hpp):

```cpp
struct Conserved {
    double kinetic_energy = 0.0;    // J
    double potential_energy = 0.0;  // J
    double total_energy = 0.0;      // J
    Vec2   momentum{};              // kg·m/s
    double angular_momentum = 0.0;  // kg·m^2/s about the origin (2D scalar)
};
```

The bookkeeping in [`world.cpp`](../../physics/src/world.cpp) is a direct transcription of
the formulas above:

```cpp
for (const Particle& p : particles_) {
    c.kinetic_energy += 0.5 * p.mass * dot(p.velocity, p.velocity);   // ½ m v²
    if (potential_energy_) c.potential_energy += potential_energy_(p);
    c.momentum         += p.velocity * p.mass;                        // Σ m v
    c.angular_momentum += p.mass * cross(p.position, p.velocity);     // Σ m (r × v)
}
c.total_energy = c.kinetic_energy + c.potential_energy;
```

Potential energy depends on the force model, so each scene supplies it. The projectile uses
$U = mgh$:

```cpp
// ch03_projectile.cpp — reset()
world_.set_potential_energy([g](const Particle& p) {
    return p.mass * g * std::max(p.position.y, 0.0);   // U = m g h
});
```

### Tested invariants

[`tests/test_physics.cpp`](../../tests/test_physics.cpp) asserts the conservation laws:

```cpp
// Momentum is conserved with no external force.
for (int i = 0; i < 100; ++i) w.step(0.01);
CHECK(w.conserved().momentum.x == doctest::Approx(expected.x));

// A symplectic integrator keeps an oscillator's energy bounded over 50 periods.
CHECK(final_energy(Integrator::Verlet) == doctest::Approx(e0).epsilon(0.01));
```

---

## 🔬 Try it

In **Projectile**, watch the **Energy (J)** plot and the `energy` readout:

1. With an accurate method (RK4) and small `dt`, total energy stays **flat** — a horizontal
   line on the plot. That's conservation of mechanical energy.
2. Switch method to **Explicit Euler** and raise `dt`. The energy line now **creeps upward**
   — Euler is injecting fake energy. (Why? See [§05](05-numerical-integration.md).)
3. The orbit scene ([§06](06-gravitation-and-orbits.md)) shows the same story spatially: the
   energy-gaining method spirals *outward*.

---

## ✅ Check yourself

<details>
<summary>1. A 1 kg ball moves at 10 m/s, 5 m above the ground (g = 9.8). Total mechanical energy?</summary>

$K = \tfrac12(1)(10^2) = 50$ J, $U = (1)(9.8)(5) = 49$ J. $E = K + U = \mathbf{99}$ J.
</details>

<details>
<summary>2. At the top of a projectile's arc, is its kinetic energy zero?</summary>

**No** — only the *vertical* velocity is zero. The horizontal velocity $v_0\cos\theta$
persists, so $K = \tfrac12 m (v_0\cos\theta)^2 > 0$ (unless launched straight up).
</details>

<details>
<summary>3. Why is angular momentum conserved in an orbit but not (about the origin) for a projectile?</summary>

Orbital gravity is a **central** force (always toward the center), so it exerts **no torque**
about that center ⇒ $L$ constant. Projectile gravity points straight down everywhere, which
*does* produce a torque about the launch origin, so $L$ about the origin changes.
</details>

<details>
<summary>4. The energy plot trends upward over time. What does that tell you?</summary>

The simulation is **gaining energy that real physics wouldn't** — a numerical artifact of
the integrator (classically Explicit Euler on oscillatory motion). Fix it with a symplectic
method (semi-implicit Euler / Verlet) or a smaller `dt`.
</details>

---

[← Newton's Laws & Acceleration](03-newtons-laws-and-acceleration.md) · [Index](README.md) · [Next: Simulating Motion Faithfully →](05-numerical-integration.md)
