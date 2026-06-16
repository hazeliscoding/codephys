# 02 · Kinematics & Projectile Motion

[← Vectors, Units & SI](01-vectors-units-and-si.md) · [Index](README.md) · [Next: Newton's Laws →](03-newtons-laws-and-acceleration.md)

---

## 🎯 Learning objectives

- Define **displacement, velocity, acceleration** and the relationships between them.
- Derive and use the **constant-acceleration** equations.
- Treat 2D projectile motion as **two independent 1D problems** (horizontal + vertical).
- Derive **range, max height, and time of flight**, and verify them against the live sim.

## 📖 Textbook map

- _College Physics 2e_ **Ch. 2** — One-dimensional kinematics.
- **Ch. 3.3–3.4** — Projectile motion.

---

## 🧠 Concepts & equations

### The kinematic chain

Velocity is the rate of change of position; acceleration is the rate of change of velocity:

$$\mathbf{v} = \frac{d\mathbf{r}}{dt}, \qquad \mathbf{a} = \frac{d\mathbf{v}}{dt}$$

For **constant acceleration** $\mathbf{a}$, integrating twice gives the equations you'll use
forever:

$$\mathbf{v}(t) = \mathbf{v}_0 + \mathbf{a}\,t$$
$$\mathbf{r}(t) = \mathbf{r}_0 + \mathbf{v}_0\,t + \tfrac{1}{2}\mathbf{a}\,t^2$$

### Projectile motion = two 1D problems

Near Earth's surface (ignoring air), the only acceleration is gravity, downward:
$\mathbf{a} = (0,\,-g)$. The horizontal and vertical motions are **independent**:

| | Horizontal $x$ | Vertical $y$ |
|---|---|---|
| acceleration | $0$ | $-g$ |
| velocity | $v_{0}\cos\theta$ (constant) | $v_{0}\sin\theta - g t$ |
| position | $v_{0}\cos\theta\,\cdot t$ | $v_{0}\sin\theta\,\cdot t - \tfrac12 g t^2$ |

### The three "money" formulas

Launching from the ground at speed $v_0$ and angle $\theta$:

**Time of flight** (solve $y=0$):

$$T = \frac{2\,v_0\sin\theta}{g}$$

**Range** (horizontal distance at $t=T$):

$$\boxed{R = \frac{v_0^2\,\sin(2\theta)}{g}}$$

**Maximum height** (where $v_y=0$):

$$H = \frac{(v_0\sin\theta)^2}{2g}$$

> [!TIP]
> Range is maximized at **θ = 45°**, because $\sin(2\theta)$ peaks at $2\theta = 90°$.
> Complementary angles (e.g. 30° and 60°) give the **same range** — a great thing to verify
> live.

---

## 💻 In the code

The projectile is a single `Particle` in a `World` whose acceleration is uniform gravity.
The scene lives in
[`app/scenes/ch03_projectile.cpp`](../../app/scenes/ch03_projectile.cpp).

```cpp
// reset(): launch a particle from the origin under constant downward gravity.
const double a = deg_to_rad(angle_deg_);
world_.set_acceleration([g](Vec2, Vec2, double) { return Vec2{0.0, -g}; });   // a = (0, -g)
world_.add(Particle{ /*pos*/ {0, 0},
                     /*vel*/ {v0 * std::cos(a), v0 * std::sin(a)},
                     /*mass*/ 1.0, {} });
```

The closed-form values are computed right next to the sim so you can compare them:

```cpp
double ProjectileScene::analytic_range() const {            // College Physics 2e §3.4
    const double a = deg_to_rad(angle_deg_);
    return v0_mps_ * v0_mps_ * std::sin(2.0 * a) / gravity_;
}
double ProjectileScene::analytic_height() const {
    const double vy = v0_mps_ * std::sin(deg_to_rad(angle_deg_));
    return vy * vy / (2.0 * gravity_);
}
```

The panel shows **predicted** (formula) vs **measured** (simulation) range — they match,
which is the whole point. The scene also draws guide lines + labels for `range` and
`max height` so the numbers map onto the picture.

### It's actually tested

[`tests/test_physics.cpp`](../../tests/test_physics.cpp) integrates the projectile with RK4
at a tiny `dt` and asserts the closed-form solution:

```cpp
const double range  = v0 * v0 * std::sin(2.0 * angle) / g;
const double height = (v0 * std::sin(angle)) * (v0 * std::sin(angle)) / (2.0 * g);
CHECK(s.position.x == doctest::Approx(range).epsilon(1e-3));   // range
CHECK(max_height   == doctest::Approx(height).epsilon(1e-3));  // apex
```

---

## 🔬 Try it

In the **Projectile** scene:

1. **Find the max-range angle.** Sweep `angle` and watch `predicted range`. Confirm it
   peaks at 45°.
2. **Complementary angles.** Set 30°, note the range; set 60°. Same range, different shape.
3. **Change gravity.** Drop `g` to ~1.6 (the Moon). The ball goes much farther — range
   scales as $1/g$.
4. **Predicted vs measured.** With method `RK4` they track tightly. Switch the method to
   `Euler` and increase `dt` (left panel) — now watch them drift apart. That's your teaser
   for [§05](05-numerical-integration.md).

---

## ✅ Check yourself

<details>
<summary>1. A ball is launched at 20 m/s, 30°, g = 9.8. What is its range?</summary>

$R = \dfrac{20^2 \sin(60°)}{9.8} = \dfrac{400 \times 0.866}{9.8} \approx \mathbf{35.3}$ m.
</details>

<details>
<summary>2. Why can we solve horizontal and vertical motion separately?</summary>

Because gravity has **no horizontal component**, so the two axes don't influence each
other. Horizontal velocity is constant; vertical velocity changes at $-g$. They share only
the same clock $t$.
</details>

<details>
<summary>3. Two launches have the same speed, at 25° and 65°. Compare their ranges.</summary>

They're **equal**: $\sin(2\cdot 25°) = \sin(50°) = \sin(130°) = \sin(2\cdot 65°)$.
Complementary angles always share a range (the 65° shot just arcs higher and slower).
</details>

<details>
<summary>4. If you double the launch speed, what happens to the range?</summary>

It **quadruples** — range goes as $v_0^2$. Doubling speed → $2^2 = 4×$ range.
</details>

---

[← Vectors, Units & SI](01-vectors-units-and-si.md) · [Index](README.md) · [Next: Newton's Laws →](03-newtons-laws-and-acceleration.md)
