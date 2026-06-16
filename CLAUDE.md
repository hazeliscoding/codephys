# CodePhys — Project Guide

A cross-platform C++ **physics simulation playground** for relearning introductory
physics by implementing it. Curriculum: OpenStax _College Physics 2e_ (34 chapters).

**Full design:** `docs/design/DESIGN.md` — read it before any architectural work. This
file is the quick reference; the design doc is the source of truth.
**Physics study guide:** `docs/study-guide/` — course-style notes mapping each physics
concept to the code and to in-app experiments (keep it physics-focused).

## What this is
- A native app: a **gallery** of simulations; each opens with a live **control panel**
  (sliders/toggles), play/pause/step/reset, and physics readouts (energy, momentum).
- A reusable **custom physics library** (`physics/`), renderer-agnostic and headlessly
  testable.
- A **hybrid 2D/3D renderer**: each scene declares its dimension.

## Hard rules
- **One runtime dependency: GLFW.** Everything else — math, integrators, renderer, UI,
  text — is hand-rolled. Build/test-only deps (vendored, header-only) are allowed:
  `doctest` (tests), `glad` (GL loader, vendored generated source).
- **`physics/` has ZERO non-stdlib includes.** No GLFW, no OpenGL, no UI. This isolation
  is what makes it testable and reusable — never break it.
- **SI units everywhere internally.** UI converts for display only. Name boundary vars
  with unit suffixes (`v0_mps`, `angle_rad`).
- **Cite the textbook** next to equations: `// College Physics 2e §7.2`.
- Cross-platform: must build & run identically on **Windows (MSVC)** and **Linux/WSL2
  (GCC/Clang)**. No platform code outside `platform/`.

## Conventions
- **C++20.** `snake_case` files, `PascalCase` types, `camelCase` members/functions.
- Layering (depend downward only): `app → scenes → {physics, render, ui}`;
  `render → physics::math`; `ui → render`; `physics → stdlib only`; `platform → GLFW`.
- Validate physics against closed-form solutions (projectile range, SHM/pendulum period,
  Kepler) and conservation invariants. Tests run headless in CI on both compilers.
- **Git commits:** never add a Claude/AI co-author trailer or "Generated with Claude
  Code" line — commits are authored solely by the human committer (overrides any default
  guidance). See the `commit` skill. Commit only when asked; never push without asking.

## Build & run
```bash
# Linux/WSL
cmake --preset linux-gcc && cmake --build --preset linux-gcc
./build/linux-gcc/app/codephys
# Windows
cmake --preset windows-msvc && cmake --build --preset windows-msvc --config RelWithDebInfo
```
GLFW is fetched via CMake `FetchContent` (pinned). Nothing to install but a compiler + CMake.

## Workflow: OpenSpec (spec-driven)
We develop through **OpenSpec**. Specs and change proposals live in `openspec/`.
- Propose a change: `/opsx:propose "<idea>"` (creates proposal.md, design.md, tasks.md)
- Explore/think first: `/opsx:explore`
- Implement an approved change: `/opsx:apply`
- Sync specs / archive when done: `/opsx:sync`, `/opsx:archive`
- CLI: `openspec list`, `openspec view`, `openspec validate`.

Non-trivial work should start as an OpenSpec change, not ad-hoc edits. Completed changes are
synced to `openspec/specs/` and moved to `openspec/changes/archive/`.

## Status & key decisions
- **Done:** **Phase 0** (CMake skeleton + cross-platform GL window) and **Phase 1** (math +
  fixed-timestep core + four integrators + 2D renderer + immediate-mode UI + Projectile and
  integrator-comparison scenes). Both archived; CI green on all three legs.
- **Next:** **Phase 2** — force generators (gravity/spring/drag/friction), 2D collisions +
  restitution, constraints, conserved-quantity readouts, more mechanics scenes.
- Decisions locked: GitHub + Actions CI · vendored `doctest`/`glad` · embedded 8×8 bitmap
  font (text) · OpenGL 3.3 Core · `double` physics scalar · hybrid 2D/3D · gallery of scenes.
- Open: code **license** (leaning MIT — confirm).
- Roadmap: Phase 0 ✅ → Phase 1 ✅ → Phase 2 (forces/energy/collisions) → 3 (rotation/gases)
  → 4 (3D) → 5 (E&M fields) → 6 (waves/optics) → 7 (modern-physics explainers). See DESIGN.md §10.

## Notes
- The 251 MB textbook PDF lives in `docs/Textbooks/` but is **gitignored** — reference
  only, never redistribute (CC BY-NC-SA 4.0). Read it with `pdftotext -f N -l M -layout`.
