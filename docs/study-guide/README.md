# CodePhys Study Guide 📚

> A course-style companion to the CodePhys engine. Each section pairs the **physics**
> (from OpenStax _College Physics 2e_) with the **code that implements it**, plus
> experiments to run in the app and self-check questions.

This guide covers everything built through **Phase 1**: vectors & units, kinematics &
projectile motion, Newton's laws, energy & momentum, numerical integration, gravitation &
orbits, the simulation loop, and the rendering/UI architecture.

---

## How to use this guide

Each chapter follows the same rhythm:

1. **🎯 Learning objectives** — what you should be able to do afterward.
2. **📖 Textbook map** — the matching OpenStax chapter(s).
3. **🧠 Concepts & equations** — the physics, with derivations where they're short.
4. **💻 In the code** — exactly where and how CodePhys implements it.
5. **🔬 Try it** — experiments to run in the app.
6. **✅ Check yourself** — questions with collapsible answers.

> [!TIP]
> Read a section, then **open the app and reproduce its claims**. The whole point of
> CodePhys is that the physics is something you can poke, not just read.
>
> ```bash
> cmake --preset linux-gcc && cmake --build --preset linux-gcc
> ./build/linux-gcc/app/codephys
> ```

---

## Curriculum map

| # | Section | Physics | OpenStax Ch. | Core code |
|---|---------|---------|--------------|-----------|
| 01 | [Vectors, Units & SI](01-vectors-units-and-si.md) | scalars vs vectors, SI units, dimensional sanity | 1, 3.2 | `physics/math/` |
| 02 | [Kinematics & Projectile Motion](02-kinematics-and-projectile-motion.md) | displacement, velocity, acceleration, parabolic flight | 2, 3 | `app/scenes/ch03_projectile` |
| 03 | [Newton's Laws & Acceleration](03-newtons-laws-and-acceleration.md) | force, mass, $F=ma$, the step pipeline | 4, 5 | `physics/core/world` |
| 04 | [Energy, Momentum & Conservation](04-energy-momentum-and-conservation.md) | KE, PE, $p$, $L$, conservation laws | 7, 8, 10 | `World::conserved()` |
| 05 | [Numerical Integration](05-numerical-integration.md) | Euler → RK4, accuracy & stability | (numerical methods) | `physics/core/integrator` |
| 06 | [Gravitation & Orbits](06-gravitation-and-orbits.md) | inverse-square gravity, Kepler, circular orbits | 6 | `app/scenes/integrator_comparison` |
| 07 | [The Simulation Loop & Time](07-the-simulation-loop-and-time.md) | discretizing time, determinism | — | `app/main.cpp` |
| 08 | [Architecture, Rendering & UI](08-architecture-rendering-and-ui.md) | how the engine is wired | — | `render/`, `ui/`, `platform/` |

---

## The big ideas (one paragraph each)

- **Everything is SI, internally.** Metres, seconds, kilograms, radians. The UI converts
  for display only. This is why a number in the sim can be compared directly to a textbook
  formula. → [§01](01-vectors-units-and-si.md)

- **Motion is an ODE.** A body obeys $\frac{d\mathbf{y}}{dt} = f(\mathbf{y}, t)$ with
  $\mathbf{y} = (\text{position}, \text{velocity})$. "Doing physics" in the engine means
  choosing the acceleration function $f$ and a method to step it forward. → [§03](03-newtons-laws-and-acceleration.md), [§05](05-numerical-integration.md)

- **The integrator is a character, not plumbing.** Explicit Euler, semi-implicit Euler,
  Velocity Verlet, and RK4 give *visibly different* answers to the same problem. Watching
  Euler spiral a planet out of its orbit while RK4 stays put **is** the lesson on numerical
  stability. → [§05](05-numerical-integration.md), [§06](06-gravitation-and-orbits.md)

- **We trust the sim because we test it.** Range, max height, orbital period, energy and
  momentum conservation, and convergence order are all asserted against closed-form
  answers in headless tests. → [§02](02-kinematics-and-projectile-motion.md), [§04](04-energy-momentum-and-conservation.md)

---

## Notation used throughout

| Symbol | Meaning | SI unit |
|--------|---------|---------|
| $\mathbf{r}$, $\mathbf{x}$ | position | m |
| $\mathbf{v}$ | velocity | m/s |
| $\mathbf{a}$ | acceleration | m/s² |
| $m$ | mass | kg |
| $\mathbf{F}$ | force | N (kg·m/s²) |
| $g$ | gravitational field strength | m/s² |
| $t$, $\Delta t$ (`dt`) | time, timestep | s |
| $\theta$ | angle | rad (deg in UI) |
| $E$, $K$, $U$ | total / kinetic / potential energy | J |
| $\mathbf{p}$, $L$ | linear / angular momentum | kg·m/s, kg·m²/s |

> [!NOTE]
> The source code can only render **ASCII**, so on-screen you'll see `T` for $\theta$ and
> `mu` for $\mu$. This guide uses proper symbols.
