# Contributing to CodePhys

CodePhys is a teaching project, so the bar for a change is simple: **the physics
must stay correct and checkable, and the architecture must stay clean.** The
authoritative rules live in [`CLAUDE.md`](CLAUDE.md) and
[`docs/design/DESIGN.md`](docs/design/DESIGN.md); this file summarises what a
contribution needs.

## Ground rules (physics & citations)

1. **Cite the source next to the equation.** Keep the textbook citation
   (`// College Physics 2e §7.2`) and, for non-trivial methods, add the
   **primary source** from [`docs/REFERENCES.md`](docs/REFERENCES.md) /
   [`REFERENCES.bib`](REFERENCES.bib) (e.g. velocity Verlet → Swope et al. 1982).
2. **Validate against closed-form solutions and conservation laws.** New physics
   needs a headless test in `tests/` checking it against an exact result
   (projectile range, SHM/pendulum period, Kepler's third law) or an invariant
   (energy, momentum, angular momentum). See `tests/test_physics.cpp`.
3. **SI units everywhere internally.** The UI converts for display only; name
   boundary variables with unit suffixes (`v0_mps`, `angle_rad`).
4. **Don't over-claim.** Use precise language — a symplectic integrator has
   *bounded energy drift*, it does not *exactly conserve* energy
   (Hairer, Lubich & Wanner 2006).

## Architecture rules (don't break these)

- **`physics/` has zero non-stdlib includes.** No GLFW, OpenGL, or UI.
- **One runtime dependency: GLFW.** Build/test-only vendored deps (`doctest`,
  `glad`) are allowed.
- **Layering, downward only:** `app → scenes → {physics, render, ui}`;
  `render → physics::math`; `ui → render`; `platform → GLFW`.
- **Cross-platform:** must build and run identically on Windows (MSVC) and
  Linux/WSL2 (GCC/Clang); no platform code outside `platform/`.

## Workflow

Non-trivial work develops through **OpenSpec** (`openspec/`): propose a change
(`/opsx:propose`), apply it (`/opsx:apply`), then sync/archive. See `CLAUDE.md`.

## Build & test (headless)

```bash
# Linux / WSL2
cmake --preset linux-gcc && cmake --build --preset linux-gcc
ctest --preset linux-gcc

# Windows (MSVC)
cmake --preset windows-msvc && cmake --build --preset windows-msvc --config RelWithDebInfo
ctest --preset windows-msvc
```

The test suite runs **headless** (no window or GPU) via doctest + CTest, and is
green on both compilers in CI ([`.github/workflows/ci.yml`](.github/workflows/ci.yml)).
What it covers:

| Test file | Checks |
|---|---|
| `tests/test_math.cpp` | vector/matrix/quaternion algebra, unit conversions |
| `tests/test_physics.cpp` | projectile range, Kepler orbit + angular-momentum conservation, integrator convergence order, symplectic energy stability, momentum conservation |

## Commits

Per [`CLAUDE.md`](CLAUDE.md): commits are authored **solely by the human
committer** — never add an AI co-author trailer or "Generated with …" line.
Commit only when asked; never push without asking.

## C++ style

C++20; `snake_case` files, `PascalCase` types, `camelCase` members/functions;
formatting via [`.clang-format`](.clang-format).
