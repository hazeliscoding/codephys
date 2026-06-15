# CodePhys 🪐

A cross-platform C++ **physics simulation playground** — relearning introductory physics
by implementing it, following OpenStax _College Physics 2e_. A gallery of interactive
simulations built on a hand-rolled, renderer-agnostic physics library.

📐 Full design: [`docs/design/DESIGN.md`](docs/design/DESIGN.md) · 📋 project rules: [`CLAUDE.md`](CLAUDE.md)

> 🚧 **Status: Phase 0** — a buildable skeleton that opens a window, brings up an OpenGL
> 3.3 Core context, and renders a "hello triangle" on Windows and Linux/WSL2. No physics yet.

## ✨ Highlights

- 🎯 **One runtime dependency: GLFW** (fetched & pinned by CMake). `glad` + `doctest` are vendored.
- 🧪 **`physics/` is stdlib-only** — layering is enforced by CMake target visibility, not convention.
- 🖥️ **Identical on Windows (MSVC) and Linux/WSL2** (GCC/Clang), verified in CI.

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
