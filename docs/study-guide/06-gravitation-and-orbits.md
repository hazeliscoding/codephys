# 06 · Gravitation & Orbits

[← Numerical Integration](05-numerical-integration.md) · [Index](README.md) · [Next: The Simulation Loop & Time →](07-the-simulation-loop-and-time.md)

---

## 🎯 Learning objectives

- State **Newton's law of universal gravitation** and the idea of an **inverse-square** force.
- Derive the **circular-orbit speed** and **Kepler's third law**.
- Connect orbital mechanics to **energy and angular-momentum conservation** ([§04](04-energy-momentum-and-conservation.md)).
- Explain what the **integrator-comparison** scene demonstrates and why.

## 📖 Textbook map

- _College Physics 2e_ **Ch. 6.5–6.6** — Newton's universal gravitation; Kepler's laws.
- **Ch. 6.2** — Centripetal acceleration.

---

## 🧠 Concepts & equations

### Universal gravitation

Every two masses attract along the line joining them, with a force that falls off as the
**square of the distance**:

$$F = \frac{G\,m_1 m_2}{r^2}$$

with $G = 6.674\times10^{-11}\ \text{N·m}^2/\text{kg}^2$. As a vector acceleration on a small
body orbiting a large central mass $M$ (writing $\mu = GM$):

$$\mathbf{a} = -\,\frac{\mu}{r^2}\,\hat{\mathbf{r}} = -\,\frac{\mu\,\mathbf{r}}{r^3}$$

The minus sign means "toward the center." This is a **central force** — always pointing at one
point — so orbits conserve angular momentum.

### Circular orbits

For a circular orbit, gravity supplies exactly the **centripetal** acceleration $v^2/r$:

$$\frac{\mu}{r^2} = \frac{v^2}{r} \;\Longrightarrow\; \boxed{v = \sqrt{\frac{\mu}{r}}}$$

The orbital **period** is the circumference over the speed:

$$T = \frac{2\pi r}{v} = 2\pi\sqrt{\frac{r^3}{\mu}}$$

### Kepler's third law

Squaring the period gives Kepler's harmonic law — period squared $\propto$ radius cubed:

$$\boxed{T^2 = \frac{4\pi^2}{\mu}\,r^3}$$

> [!TIP]
> A circular orbit is a perfect physics *and* numerics test: the analytic period is known
> exactly, energy and angular momentum are both conserved, and any integrator error shows up
> immediately as the orbit failing to close.

---

## 💻 In the code

The **Integrators (orbit)** scene
([`app/scenes/integrator_comparison.cpp`](../../app/scenes/integrator_comparison.cpp)) seeds
the *same* circular orbit into two `World`s — one stepped with Explicit Euler, one with RK4:

```cpp
// Central inverse-square acceleration toward the origin: a = -mu * r / |r|^3.
AccelFn central_accel(double mu) {
    return [mu](Vec2 p, Vec2, double) {
        const double r = length(p);
        return r > 0 ? p * (-mu / (r * r * r)) : Vec2{};
    };
}

// Seed a circular orbit: speed v = sqrt(mu / r), perpendicular to the radius.
void seed_orbit(World& world, Integrator scheme, double mu, double radius) {
    world.set_acceleration(central_accel(mu));
    const double speed = std::sqrt(mu / radius);
    world.add(Particle{ {radius, 0}, {0, speed}, 1.0, {} });
}
```

Both worlds advance under the identical force; only the integrator differs. Euler (red)
slowly **spirals outward** because it gains energy; RK4 (green) holds the circle. This is the
[§05](05-numerical-integration.md) lesson made spatial.

### Kepler is tested

[`tests/test_physics.cpp`](../../tests/test_physics.cpp) integrates one full Kepler period and
checks the body returns to its start, with angular momentum conserved:

```cpp
const double speed  = std::sqrt(mu / radius);                       // circular-orbit speed
const double period = 2.0 * pi * std::sqrt(radius*radius*radius / mu);  // Kepler's 3rd law
// ... integrate one period with RK4 ...
CHECK(s.position.x == doctest::Approx(radius).epsilon(1e-3));       // returned to start
CHECK(cross(s.position, s.velocity) == doctest::Approx(l0).epsilon(1e-6));  // L conserved
```

---

## 🔬 Try it

In **Integrators (orbit)**:

1. **Watch the divergence.** Let it run. Red (Euler) winds outward; green (RK4) stays on the
   circle. Same starting point, same gravity — only the math differs.
2. **Speed it up.** Raise **time scale** (left panel) to fast-forward many orbits and make the
   spiral obvious in seconds.
3. **Make Euler "work."** Drop **`dt`** way down. With a tiny step, even Euler tracks the orbit
   for a while — accuracy improves as $\Delta t \to 0$, but it still drifts eventually.
4. **Reset** re-seeds both from the identical circular state.

---

## ✅ Check yourself

<details>
<summary>1. A satellite orbits at radius r with μ = GM. If you double r, what happens to the period?</summary>

$T \propto r^{3/2}$, so doubling $r$ multiplies the period by $2^{3/2} \approx \mathbf{2.83}$.
(Kepler's third law.)
</details>

<details>
<summary>2. Why does the orbiting body need a sideways velocity √(μ/r) rather than zero?</summary>

With zero velocity it would just fall straight into the center. A circular orbit needs the
tangential speed where gravity provides exactly the centripetal acceleration:
$\mu/r^2 = v^2/r \Rightarrow v = \sqrt{\mu/r}$.
</details>

<details>
<summary>3. The orbit is a central force. Which conservation law is therefore guaranteed?</summary>

**Angular momentum.** A central force points at the center, so its torque about the center is
zero, so $L$ is conserved (Kepler's "equal areas" law).
</details>

<details>
<summary>4. Both worlds use identical physics. So why do the trajectories differ?</summary>

Only the **integrator** differs. Explicit Euler accumulates a one-sided energy error and
spirals out; RK4 is accurate enough to keep the circle. The scene isolates the *numerical
method* as the single variable.
</details>

---

[← Numerical Integration](05-numerical-integration.md) · [Index](README.md) · [Next: The Simulation Loop & Time →](07-the-simulation-loop-and-time.md)
