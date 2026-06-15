## Why

CodePhys has a design (`docs/design/DESIGN.md`) but no code. The single biggest
unknown is **cross-platform parity**: will an OpenGL 3.3 Core context come up and render
identically on Windows (MSVC) and Linux/WSL2 (under WSLg)? Every later phase is built on
top of that assumption, so we de-risk it first. This change is **Phase 0** of the
roadmap (DESIGN.md §10): a buildable, runnable skeleton that opens a window and proves
the rendering pipeline works on both platforms — before any physics is written.

## What Changes

- Establish the **CMake build** (`CMake >= 3.21`) with `CMakePresets.json` for
  `windows-msvc`, `linux-gcc`, and `linux-clang`. C++20, warnings-as-errors.
- Create the **repository module skeleton** (empty-but-wired targets): `physics/`,
  `render/`, `ui/`, `platform/`, `app/`, `tests/`, `third_party/` — matching DESIGN §5.
  Enforce the dependency direction so `physics/` cannot link GLFW/OpenGL.
- Manage dependencies per the one-runtime-dep rule: **GLFW 3.4** via CMake
  `FetchContent` (pinned tag); **glad** (vendored generated GL loader) and **doctest**
  (vendored header) under `third_party/`.
- Stand up the **platform layer**: open a window, create an OpenGL 3.3 Core context,
  run a basic event loop (poll, swap, clean shutdown), and report GL/driver info.
- **Smoke test**: clear the framebuffer and draw a single triangle (minimal shader +
  vertex buffer) to prove the GL pipeline end-to-end. The `app` target runs this.
- Wire a **doctest** test target with one trivial passing test so `ctest` works on both
  platforms; add a **GitHub Actions** matrix (windows-msvc, ubuntu-gcc, ubuntu-clang)
  that configures, builds warnings-as-errors, and runs tests.
- Add `.clang-format` and a `README.md` with build/run instructions for both platforms.

No new runtime dependency beyond GLFW. glad and doctest are vendored, build/test-time
only, and ship nothing into the app binary — consistent with the one-runtime-dep rule.

## Capabilities

### New Capabilities
- `build-system`: Cross-platform CMake configuration, presets, C++20 + warnings-as-errors,
  dependency acquisition (FetchContent for GLFW; vendored glad/doctest), the module-target
  skeleton with enforced layering, the test harness (doctest + ctest), and CI.
- `platform-window`: A GLFW-backed window/context/input layer that opens a window, creates
  an OpenGL 3.3 Core context, runs the event loop with clean shutdown, exposes GL/driver
  info, and demonstrates a working pipeline via a triangle smoke-test render — verified
  identical on Windows and Linux/WSL2.

### Modified Capabilities
<!-- None — this is the first change; no existing specs. -->

## Impact

- **New code:** top-level `CMakeLists.txt`, `CMakePresets.json`, `.clang-format`,
  `.github/workflows/ci.yml`, `README.md`, and the module skeleton (`physics/`, `render/`,
  `ui/`, `platform/`, `app/`, `tests/`, `third_party/`).
- **Dependencies:** GLFW 3.4 (runtime, fetched); glad + doctest (vendored, build/test-time).
- **Systems:** Establishes the build/CI substrate every later phase depends on. The
  layering rules become physically enforced (not just convention) via target link scoping.
- **Risk addressed:** OpenGL-under-WSLg uncertainty (DESIGN §13) — surfaced and resolved
  here, with a documented software-GL fallback if needed.

## Non-goals

- **No physics.** `physics/` is created as an empty, stdlib-only target with a placeholder
  test; no integrators, bodies, or math beyond what compiles.
- **No real renderer or UI.** `render/` and `ui/` are wired but minimal; the triangle is a
  throwaway smoke test, not the Phase 1 renderer.
- **Text rendering is deferred to Phase 1** (the bitmap/SDF font belongs with the real
  renderer; building it twice is waste). Phase 0 proves the GL context, not typography.
- **No scenes, gallery, time control, or interactivity** — those are Phase 1+.
- **No performance work** (batching, instancing) — Phase 1+.
