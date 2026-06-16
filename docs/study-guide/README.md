# CodePhys Study Guide 📚

> A **physics** course companion to the CodePhys engine. Each section teaches a physics topic
> from OpenStax _College Physics 2e_, then shows briefly how CodePhys lets you *see* it and
> where the relevant code lives — so you can poke the physics, not just read it.

This guide is **about the physics**. For the engine internals (rendering, UI, the simulation
loop, the layered architecture) see [`docs/design/DESIGN.md`](../design/DESIGN.md). For the
**primary-literature sources** behind each law, method, and constant, see
[`docs/REFERENCES.md`](../REFERENCES.md).

---

## How to use this guide

Each chapter follows the same rhythm:

1. **🎯 Learning objectives** — what you should be able to do afterward.
2. **📖 Textbook map** — the matching OpenStax chapter(s).
3. **🧠 Concepts & equations** — the physics, with short derivations.
4. **🔬 Try it** — experiments to run in the app.
5. **✅ Check yourself** — questions with collapsible answers.

A short **"in the code"** note appears where it helps connect a formula to what you can run,
but the focus is the physics.

> [!TIP]
> Read a section, then **open the app and reproduce its claims**.
>
> ```bash
> cmake --preset linux-gcc && cmake --build --preset linux-gcc
> ./build/linux-gcc/app/codephys
> ```

---

## Curriculum map

| # | Section | Physics | OpenStax Ch. |
|---|---------|---------|--------------|
| 01 | [Vectors, Units & SI](01-vectors-units-and-si.md) | scalars vs vectors, SI units, dimensional sense | 1, 3.2 |
| 02 | [Kinematics & Projectile Motion](02-kinematics-and-projectile-motion.md) | displacement, velocity, acceleration, parabolic flight | 2, 3 |
| 03 | [Newton's Laws & Acceleration](03-newtons-laws-and-acceleration.md) | force, mass, $F=ma$ | 4, 5 |
| 04 | [Energy, Momentum & Conservation](04-energy-momentum-and-conservation.md) | KE, PE, momentum, angular momentum, conservation | 7, 8, 10 |
| 05 | [Simulating Motion Faithfully](05-numerical-integration.md) | why simulated energy/orbits drift, and how to keep them honest | 6–8 (applied) |
| 06 | [Gravitation & Orbits](06-gravitation-and-orbits.md) | inverse-square gravity, circular orbits, Kepler | 6 |

---

## The big ideas (one paragraph each)

- **Everything is SI.** Metres, seconds, kilograms, radians — so a number in the sim can be
  compared directly to a textbook formula. → [§01](01-vectors-units-and-si.md)

- **Projectile motion is two 1D problems.** Horizontal motion is constant-velocity; vertical
  motion is constant-acceleration under gravity. They share only a clock. → [§02](02-kinematics-and-projectile-motion.md)

- **Force sets acceleration.** Newton's second law $\mathbf{a} = \mathbf{F}/m$ is the rule the
  whole engine runs on: know the force, get the acceleration, advance the motion. → [§03](03-newtons-laws-and-acceleration.md)

- **Conservation laws are the truth test.** With no friction, mechanical energy is conserved;
  with no external force, momentum is; with no torque, angular momentum is. If the sim breaks
  one of these when it shouldn't, something is wrong. → [§04](04-energy-momentum-and-conservation.md)

- **A simulation only approximates the physics.** How you step time decides whether energy is
  conserved and whether an orbit stays an orbit. Watching a planet spiral away is a physics
  lesson about *faithfulness*, not a bug. → [§05](05-numerical-integration.md)

- **Gravity is inverse-square and central.** That single fact gives circular-orbit speed,
  Kepler's third law, and conserved angular momentum. → [§06](06-gravitation-and-orbits.md)

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
> The app can only render **ASCII**, so on-screen you'll see `T` for $\theta$ and `mu` for
> $\mu$. This guide uses proper symbols.
