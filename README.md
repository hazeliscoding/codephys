# CodePhys 🪐

A cross-platform C++ **physics simulation playground** — relearning introductory physics
by implementing it, following OpenStax _College Physics 2e_. A gallery of interactive
simulations built on a hand-rolled, renderer-agnostic physics library.

📐 Full design: [`docs/design/DESIGN.md`](docs/design/DESIGN.md) · 📋 project rules: [`CLAUDE.md`](CLAUDE.md)

> 🚀 **Status: Phase 1** — the engine foundation: a `physics::math` library, a deterministic
> fixed-timestep core with four switchable integrators, a hand-rolled 2D renderer + bitmap-font
> text, an immediate-mode UI, and a scene gallery. Flagship scenes: **Projectile (Ch 3)** and an
> **integrator comparison** (Euler vs RK4 on an orbit). Runs on Windows and Linux/WSL2.

## ✨ Highlights

- 🎯 **One runtime dependency: GLFW** (fetched & pinned by CMake). `glad` + `doctest` are vendored.
- 🧪 **`physics/` is stdlib-only** — layering is enforced by CMake target visibility, not convention.
- 🔢 **Four switchable integrators** (Explicit/Semi-implicit Euler, Velocity Verlet, RK4),
  validated headless against closed-form solutions (projectile range, Kepler) and conservation laws.
- 🖥️ **Identical on Windows (MSVC) and Linux/WSL2** (GCC/Clang), verified in CI.

## 🎮 Scenes & controls

- **Projectile (Ch 3)** — sliders for launch speed, angle, and gravity; live trajectory,
  velocity vector, energy plot, and `R = v0²·sin(2θ)/g` readout.
- **Integrators (orbit)** — the same circular-orbit start under Explicit Euler (red, spirals out)
  and RK4 (green, stable).
- **Time control** — Play/Pause (or **space**), **Step** one tick, **Reset**, plus time-scale and
  `dt` sliders (crank `dt` to watch a scheme go unstable). **Esc** quits.

## 🔧 Prerequisites

- **CMake ≥ 3.21** and a **C++20 compiler** (Visual Studio 2022, or GCC/Clang).
- **Linux/WSL2** also needs GLFW's build deps: `sudo apt-get install build-essential cmake libgl1-mesa-dev xorg-dev`
- The first configure downloads GLFW, so it needs network access. Nothing else to install.

## 🚀 Build & run

**Linux / WSL2**
```bash
cmake --preset linux-gcc          # or linux-clang
cmake --build --preset linux-gcc
./build/linux-gcc/app/codephys
```

**Windows (MSVC)**
```powershell
cmake --preset windows-msvc
cmake --build --preset windows-msvc --config RelWithDebInfo
.\build\windows-msvc\app\RelWithDebInfo\codephys.exe
```

A window opens, the OpenGL version/renderer/GLSL strings print, and a triangle draws.
Close the window or press **Esc** to exit.

## 🧪 Tests

Headless (no window or GPU), via CTest:
```bash
ctest --preset linux-gcc          # or windows-msvc
```

## 🐧 WSLg / software-GL note

WSLg renders GUI apps natively, typically via Mesa's software renderer (`llvmpipe`), which
provides OpenGL ≥ 4.5 — well past our 3.3 floor. If a context fails, force software GL:
```bash
LIBGL_ALWAYS_SOFTWARE=1 ./build/linux-gcc/app/codephys
```

## 🗂️ Layout

| Dir          | Role                                                       |
| ------------ | --------------------------------------------------------- |
| `physics/`   | The physics library — **stdlib only**, no rendering deps. |
| `render/`    | 2D/3D renderer over OpenGL (uses `physics::math`).         |
| `ui/`        | Hand-rolled immediate-mode UI (uses `render`).            |
| `platform/`  | GLFW window, OpenGL context, input, event loop.           |
| `app/`       | The playground executable.                                 |
| `tests/`     | Headless tests (doctest + CTest).                          |
| `third_party/` | Vendored `glad` + `doctest`; GLFW fetched here.         |

Dependencies flow one way: `app → {physics, render, ui, platform}`, `render → physics::math`,
`ui → render`, `platform → GLFW`, `physics → stdlib only`.
