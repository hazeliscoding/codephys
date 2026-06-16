# CodePhys 🪐

A cross-platform C++ **physics simulation playground** — relearning introductory physics
by implementing it, following OpenStax _College Physics 2e_. A gallery of interactive
simulations built on a hand-rolled, renderer-agnostic physics library.

📐 Full design: [`docs/design/DESIGN.md`](docs/design/DESIGN.md) · 📋 project rules: [`CLAUDE.md`](CLAUDE.md) · 📚 study guide: [`docs/study-guide/`](docs/study-guide/README.md) · 📑 references: [`docs/REFERENCES.md`](docs/REFERENCES.md)

[![CI](https://img.shields.io/badge/CI-Windows%20%7C%20Linux-blue)](.github/workflows/ci.yml) [![tests: doctest + CTest](https://img.shields.io/badge/tests-doctest%20%2B%20CTest-blue)](tests/) [![license: MIT](https://img.shields.io/badge/license-MIT-green)](LICENSE) [![College Physics 2e](https://img.shields.io/badge/textbook-OpenStax%20College%20Physics%202e-orange)](https://openstax.org/details/books/college-physics-2e)

> 🚀 **Status: Phase 1** — the engine foundation: a `physics::math` library, a deterministic
> fixed-timestep core with four switchable integrators, a hand-rolled 2D renderer + bitmap-font
> text, an immediate-mode UI, and a scene gallery. Flagship scenes: **Projectile (Ch 3)** and an
> **integrator comparison** (Euler vs RK4 on an orbit). Runs on Windows and Linux/WSL2.

## ✨ Highlights

- 🎯 **One runtime dependency: GLFW** (fetched & pinned by CMake). `glad` + `doctest` are vendored.
- 🧪 **`physics/` is stdlib-only** — layering is enforced by CMake target visibility, not convention.
- 🔢 **Four switchable integrators** — Explicit Euler ([Euler 1768](docs/REFERENCES.md)),
  Semi-implicit/symplectic Euler ([Störmer 1907](docs/REFERENCES.md)), Velocity Verlet
  ([Verlet 1967](docs/REFERENCES.md); [Swope et al. 1982](docs/REFERENCES.md)), and classical
  RK4 ([Runge 1895](docs/REFERENCES.md); [Kutta 1901](docs/REFERENCES.md)) — validated headless
  against closed-form solutions (projectile range, Kepler) and conservation laws.
- 📖 **Mapped to the literature.** Every law, method, and constant cites a section of _College
  Physics 2e_ **and its primary source** — see [`docs/REFERENCES.md`](docs/REFERENCES.md).
  Constants are exact CODATA 2018 / SI values.
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

A window opens with the **scene gallery** — pick a scene on the left, press Play, and drag
the sliders. Close the window or press **Esc** to exit.

## 📚 Learn the physics

CodePhys ships a **physics study guide** in [`docs/study-guide/`](docs/study-guide/README.md):
a course-style set of notes (OpenStax _College Physics 2e_) that pairs each concept with an
experiment to run in the app.

Vectors & units · Kinematics & projectile motion · Newton's laws · Energy, momentum &
conservation · Simulating motion faithfully · Gravitation & orbits.

## 🧪 Tests

The physics is checkable: the integrators and physics models are **validated headless** (no
window or GPU) against closed-form solutions and conservation laws, on both compilers in CI.

```bash
ctest --preset linux-gcc          # or windows-msvc
```

| Test file | Asserts |
|---|---|
| `tests/test_math.cpp` | vector / matrix / quaternion algebra · unit conversions |
| `tests/test_physics.cpp` | projectile range `R = v0²·sin(2θ)/g` · Kepler orbit + angular-momentum conservation · integrator convergence order (Euler ≈ 1st, RK4 ≈ 4th) · symplectic energy stability (Verlet/semi-implicit bounded, explicit Euler gains) · momentum conservation |

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
| `docs/REFERENCES.md` · `REFERENCES.bib` | Primary-literature bibliography; which source backs which code. |
| `CITATION.cff` · `LICENSE` · `CONTRIBUTING.md` | Citation metadata, MIT license, contribution guide. |

Dependencies flow one way: `app → {physics, render, ui, platform}`, `render → physics::math`,
`ui → render`, `platform → GLFW`, `physics → stdlib only`.

## 📑 References & reproducibility

- **Primary literature.** Each integrator, law, and constant is traced to its source in
  [`docs/REFERENCES.md`](docs/REFERENCES.md), with machine-readable BibTeX in
  [`REFERENCES.bib`](REFERENCES.bib). Journal/book DOIs were verified against CrossRef.
- **Reproducible builds.** CMake presets pin the toolchain and fetch a pinned GLFW; the
  physics scalar is `double` and the core is a deterministic fixed timestep, so the headless
  tests are reproducible across Windows (MSVC) and Linux/WSL2 (GCC/Clang) — verified in CI.

## 📝 Citing CodePhys

If CodePhys helps your teaching or research, cite it via [`CITATION.cff`](CITATION.cff)
(GitHub's "Cite this repository" button reads it). Please also cite the underlying primary
sources and OpenStax _College Physics 2e_ listed in [`docs/REFERENCES.md`](docs/REFERENCES.md).

## ⚖️ License

Code is licensed under the [MIT License](LICENSE). The OpenStax _College Physics 2e_ textbook
PDF referenced for the curriculum is **not** part of this repository (it is gitignored) and is
separately licensed CC BY-NC-SA 4.0 by OpenStax — reference only, not redistributed.
