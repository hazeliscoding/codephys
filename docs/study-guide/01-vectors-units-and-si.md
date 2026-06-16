# 01 · Vectors, Units & SI

[← Index](README.md) · [Next: Kinematics & Projectile Motion →](02-kinematics-and-projectile-motion.md)

---

## 🎯 Learning objectives

By the end you can:

- Distinguish **scalars** from **vectors** and operate on 2D/3D vectors (add, scale, dot, cross, normalize).
- State the **SI base units** relevant to mechanics and explain why CodePhys is "SI everywhere internally."
- Convert between **degrees and radians** and know which one the math actually uses.
- Read the engine's math types and predict what each operation returns.

## 📖 Textbook map

- _College Physics 2e_ **Ch. 1** — Units, unit conversion, significant figures.
- **Ch. 3.2** — Vector addition/subtraction; components.

---

## 🧠 Concepts & equations

### Scalars vs vectors

A **scalar** has magnitude only (mass `5 kg`, time `2 s`). A **vector** has magnitude *and*
direction (velocity `(3, 4) m/s`). In 2D we write $\mathbf{v} = (v_x, v_y)$.

| Operation | Definition | Returns |
|-----------|------------|---------|
| Addition | $\mathbf{a}+\mathbf{b} = (a_x+b_x,\ a_y+b_y)$ | vector |
| Scaling | $s\,\mathbf{a} = (s\,a_x,\ s\,a_y)$ | vector |
| Magnitude | $\lvert\mathbf{a}\rvert = \sqrt{a_x^2 + a_y^2}$ | scalar |
| Dot product | $\mathbf{a}\cdot\mathbf{b} = a_x b_x + a_y b_y = \lvert a\rvert\lvert b\rvert\cos\phi$ | scalar |
| Cross (2D) | $\mathbf{a}\times\mathbf{b} = a_x b_y - a_y b_x$ | scalar (the $z$ of the 3D cross) |
| Normalize | $\hat{\mathbf{a}} = \mathbf{a}/\lvert\mathbf{a}\rvert$ | unit vector |

> [!NOTE]
> The **dot product** measures "how aligned" two vectors are (zero ⇒ perpendicular). The
> **cross product** measures "how perpendicular" they are, and in 2D its sign tells you the
> rotation direction (CCW positive). Both show up constantly: dot for work and projections,
> cross for torque and angular momentum.

### SI units

CodePhys is **SI internally** — every stored quantity is in base SI units:

| Quantity | Unit | Symbol |
|----------|------|--------|
| length | metre | m |
| mass | kilogram | kg |
| time | second | s |
| angle | radian | rad |
| force | newton | N = kg·m/s² |
| energy | joule | J = kg·m²/s² |

Why be strict? Because then a simulated number can be compared **directly** to a textbook
formula with no conversion fudge. A projectile launched at `30` (m/s) and `0.785` (rad)
should land at exactly $R = v_0^2\sin(2\theta)/g$ metres — and it does (see [§02](02-kinematics-and-projectile-motion.md)).

### Degrees vs radians

Humans think in degrees; trig functions think in radians.

$$\theta_{\text{rad}} = \theta_{\text{deg}} \cdot \frac{\pi}{180}, \qquad \theta_{\text{deg}} = \theta_{\text{rad}} \cdot \frac{180}{\pi}$$

The rule in CodePhys: **degrees only at the UI boundary, radians everywhere else.** Boundary
variables even carry the unit in their name (`angle_deg`, `angle_rad`).

---

## 💻 In the code

The math module is header-only `constexpr` value types in
[`physics/include/physics/math/`](../../physics/include/physics/math/), and it depends on
**nothing but the C++ standard library** — a hard rule that keeps physics testable and
renderer-agnostic.

```cpp
// physics/math/vec2.hpp — a 2D vector is just two doubles + free functions.
struct Vec2 { double x = 0.0, y = 0.0; };

constexpr Vec2 operator+(Vec2 a, Vec2 b) { return {a.x + b.x, a.y + b.y}; }
constexpr double dot(Vec2 a, Vec2 b)     { return a.x * b.x + a.y * b.y; }
constexpr double cross(Vec2 a, Vec2 b)   { return a.x * b.y - a.y * b.x; }  // scalar (z of 3D cross)
inline   double length(Vec2 v)           { return std::sqrt(dot(v, v)); }
inline   Vec2   normalize(Vec2 v)        { double L = length(v); return L > 0 ? v / L : Vec2{}; }
```

| Type | File | Notes |
|------|------|-------|
| `Vec2` / `Vec3` / `Vec4` | `math/vec2.hpp` … `vec4.hpp` | `double`, aggregate init, full operator set |
| `Mat3` / `Mat4` | `math/mat3.hpp`, `mat4.hpp` | column-major, multiply / transpose / `inverse`, `ortho` |
| `Quat` | `math/quat.hpp` | orientation (used heavily in a later 3D phase) |
| constants | `math/constants.hpp` | SI, each cited to the textbook |
| conversions | `math/units.hpp` | `deg_to_rad`, `rad_to_deg` |

> [!IMPORTANT]
> The scalar type is **`double`**, not `float`. Accuracy and clean cross-compiler test
> results matter more than raw speed here — the math *is* the learning material, so it
> favors clarity (see DESIGN §8.1).

### Constants (all SI, all cited)

```cpp
// physics/math/constants.hpp (excerpt)
inline constexpr double g_earth = 9.80665;     // standard gravity, m/s^2   (CP2e §1.2)
inline constexpr double G       = 6.67430e-11; // gravitational constant     (CP2e §6.5)
inline constexpr double c_light = 2.99792458e8;// speed of light, m/s        (CP2e §1.2)
inline constexpr double pi      = 3.14159265358979323846;
```

### Why `cross` returns a scalar in 2D

The full 3D cross of two vectors in the $xy$-plane points along $z$:
$\mathbf{a}\times\mathbf{b} = (0,\,0,\,a_x b_y - a_y b_x)$. Only that $z$-component is
non-zero, so 2D code keeps just the number. You'll use it directly for **angular momentum**
$L = m(\mathbf{r}\times\mathbf{v})$ in [§04](04-energy-momentum-and-conservation.md).

---

## 🔬 Try it

These are exercised by the unit tests rather than the GUI — run them and read them:

```bash
ctest --preset linux-gcc --output-on-failure      # or windows-msvc
```

Open [`tests/test_math.cpp`](../../tests/test_math.cpp) and confirm each assertion matches
the table above (dot, cross, matrix inverse round-trip, deg↔rad round-trip).

---

## ✅ Check yourself

<details>
<summary>1. A ball has velocity <code>(3, 4) m/s</code>. What is its speed?</summary>

$\lvert\mathbf{v}\rvert = \sqrt{3^2 + 4^2} = \sqrt{25} = \mathbf{5}$ m/s. In code:
`length(Vec2{3, 4}) == 5`.
</details>

<details>
<summary>2. What does <code>dot(a, b) == 0</code> tell you about <code>a</code> and <code>b</code>?</summary>

They are **perpendicular** (assuming neither is the zero vector), because
$\mathbf{a}\cdot\mathbf{b} = \lvert a\rvert\lvert b\rvert\cos\phi$ and $\cos 90° = 0$.
</details>

<details>
<summary>3. Convert 45° to radians. Why does the engine bother?</summary>

$45 \cdot \pi/180 = \pi/4 \approx 0.785$ rad. The engine works in radians because
`std::sin`/`std::cos` expect radians; degrees only exist at the UI so humans can read them.
</details>

<details>
<summary>4. Why is <code>physics/</code> forbidden from including GLFW or OpenGL?</summary>

So the physics library stays **headlessly testable and reusable**. If it never touches the
window/GPU, the entire test suite runs on a CI machine with no display — and the physics
could be dropped into a totally different renderer unchanged.
</details>

---

[← Index](README.md) · [Next: Kinematics & Projectile Motion →](02-kinematics-and-projectile-motion.md)
