## Context

CodePhys is greenfield — no code, only `docs/design/DESIGN.md`. Phase 0 stands up the
build substrate and proves a cross-platform OpenGL 3.3 Core context. The dominant
constraint is **parity between Windows (MSVC) and Linux/WSL2 (WSLg)**, plus the project's
hard rules: one runtime dependency (GLFW), `physics/` stdlib-only, strict layering. This
design covers the build topology, dependency handling, and the platform-layer shape —
the decisions worth settling before writing the skeleton. It is cross-cutting (touches
every module target and introduces external dependencies), which is why a design doc is
warranted.

## Goals / Non-Goals

**Goals:**
- One CMake build, three presets, identical results on Windows and Linux/WSL.
- Module-target skeleton (`physics`, `render`, `ui`, `platform`, `app`, `tests`) with
  layering enforced by target link/include visibility — not just convention.
- GLFW fetched reproducibly; glad + doctest vendored; GLFW is the only runtime dep.
- A `platform` layer that owns the window, GL context, and event loop, exposing a tiny
  surface to `app`; `app` runs a triangle smoke test.
- CI matrix (windows-msvc, ubuntu-gcc, ubuntu-clang) green on configure + build + test.

**Non-Goals:**
- Any physics, real renderer, UI widgets, scenes, or text rendering (Phase 1+).
- GL abstraction beyond what the smoke test needs. The triangle code is disposable.
- Performance, batching, or instancing.

## Decisions

### D1 — Dependency strategy: FetchContent for GLFW, vendored glad/doctest
GLFW via CMake `FetchContent` pinned to a tag keeps the repo small and the version
explicit, while needing no system package manager. glad is a *generated* loader, so we
vendor the generated `.c/.h` under `third_party/glad/` (committed) — it isn't a managed
dependency and pinning the generated output is the reproducible choice. doctest is a
single header, vendored under `third_party/doctest/`.
- *Alternatives considered:* git submodules (more setup friction, easy to forget
  `--recursive`); system-installed GLFW via `find_package` (breaks the "no manual install"
  goal and drifts across machines); vcpkg/Conan (adds a package-manager dependency that
  fights the minimal-deps principle). FetchContent + vendoring wins on simplicity.

### D2 — Layering enforced physically via target visibility
Each module is its own CMake target. Dependencies are expressed with
`target_link_libraries(... PRIVATE/PUBLIC ...)` and include dirs scoped with
`PUBLIC`/`PRIVATE` so that, e.g., `physics` never receives GLFW/OpenGL include paths or
link deps. `physics::math` is exposed as the only part of `physics` that `render` sees
(via a small interface/`PUBLIC` include surface). This makes "physics stays stdlib-only"
a build error to violate, not a code-review hope.
- *Alternatives considered:* a single monolithic target (fast but no enforcement);
  header-only modules (loses link-time isolation). Per-module targets give us enforcement
  with little overhead.

### D3 — Platform layer surface
`platform/` wraps GLFW behind a minimal C++ API: a `Window` type (RAII over GLFW init,
window, and GL context; loads glad), a run-loop entry that takes a per-frame callback,
and a GL-info query. `app/main.cpp` constructs a `Window`, registers a render callback
that clears + draws the triangle, and runs the loop. This keeps all GLFW/OpenGL surface
inside `platform/` (and the throwaway triangle in `app/`), so no other module learns
about the windowing system.
- *Alternatives considered:* `app` calling GLFW directly (leaks platform concerns into
  `app`); a full renderer abstraction now (premature — that's Phase 1). A thin wrapper is
  the right amount for Phase 0.

### D4 — OpenGL 3.3 Core + glad loader
Matches DESIGN §3: ubiquitous, works under WSLg, minimal to bring up. Request a 3.3 Core
profile context via GLFW window hints; load with glad. GL/driver strings are logged at
startup to make WSLg/software-GL differences visible.
- *Alternatives considered:* Vulkan/WebGPU (rejected in DESIGN — too much boilerplate,
  fights the learning goal).

### D5 — Warnings-as-errors scoped to first-party code only
Apply the strict warning flags via a small interface target (e.g.
`codephys_warnings`) linked `PRIVATE` into first-party targets; vendored/fetched code is
compiled without it (FetchContent's GLFW and the vendored headers are excluded). Flags:
MSVC `/W4 /WX`; GCC/Clang `-Wall -Wextra -Wpedantic -Werror`.

### D6 — Headless tests
`tests` links doctest and `physics` only (never `platform`/GLFW), so CI can run `ctest`
on machines with no display/GPU. Phase 0 ships one trivial test to prove the harness;
real tests arrive with `physics::math` in Phase 1.

## Risks / Trade-offs

- **OpenGL under WSLg is software/virtual or wrong version** → Log GL info at startup;
  document a fallback (request compatibility hints / `LIBGL_ALWAYS_SOFTWARE=1`,
  install `mesa-utils`/`libgl1`) in the README; verify on a real WSL2 box as a task gate.
- **FetchContent first-configure needs network / is slow** → Pin the tag; document that
  the first configure downloads GLFW; CI caches the build dir where possible.
- **CRLF/EOL churn across platforms** → already mitigated by `.gitattributes` (LF
  normalization) added during repo setup.
- **glad generated for the wrong GL version/profile** → Vendor a glad generated for GL
  3.3 Core explicitly; record the generation parameters in `third_party/glad/README`.
- **Layering enforcement is only as good as the target setup** → Keep module targets
  small and review `target_link_libraries` visibility; the headless `tests` target acts
  as a canary (it must build without GLFW).

## Migration Plan

Greenfield — nothing to migrate or roll back. Deployment = developers run the documented
preset commands. If WSLg GL proves unworkable on a given machine, the software-GL
fallback is the rollback for the rendering smoke test; the build/test substrate is
unaffected.

## Open Questions

- **Code license** — leaning MIT; to confirm before first public push (does not block
  Phase 0 implementation).
- **Exact GLFW pin** — pick the latest stable 3.4.x tag at implementation time.
- **CI cache granularity** — whether to cache FetchContent downloads / build dirs now or
  defer until build times warrant it.
