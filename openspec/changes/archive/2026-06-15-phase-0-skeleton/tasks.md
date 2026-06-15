## 1. Build scaffolding

- [x] 1.1 Add top-level `CMakeLists.txt`: `cmake_minimum_required(VERSION 3.21)`, project name/version, C++20 standard (required, no extensions), enable testing
- [x] 1.2 Add `CMakePresets.json` with `windows-msvc`, `linux-gcc`, `linux-clang` (binary dirs under `build/<preset>/`)
- [x] 1.3 Add `codephys_warnings` INTERFACE target with MSVC `/W4 /WX` and GCC/Clang `-Wall -Wextra -Wpedantic -Werror`; link PRIVATE into first-party targets only
- [x] 1.4 Add `.clang-format` (style matching DESIGN conventions) and confirm it formats a sample file

## 2. Dependencies

- [x] 2.1 Acquire GLFW 3.4 via `FetchContent` pinned to a specific stable tag; disable GLFW docs/tests/examples; exclude from warnings-as-errors
- [x] 2.2 Vendor glad (GL 3.3 Core) generated loader under `third_party/glad/` (src + include); add `third_party/glad/README` recording generation params
- [x] 2.3 Vendor doctest single header under `third_party/doctest/`
- [x] 2.4 Verify a clean configure fetches GLFW reproducibly on both platforms <!-- GLFW 3.4 fetched & built on Windows (MSVC) and Linux (GCC) -->

## 3. Module skeleton with enforced layering

- [x] 3.1 Create `physics/` target (stdlib-only library; one placeholder header in `physics::math`), with NO link/include to GLFW/OpenGL/glad
- [x] 3.2 Create `render/`, `ui/` targets (minimal stubs) with correct PUBLIC/PRIVATE visibility (`render` sees `physics::math`; `ui` sees `render`)
- [x] 3.3 Create `platform/` target linking GLFW + glad
- [x] 3.4 Create `app/` executable (`codephys`) depending on `physics`, `render`, `ui`, `platform`
- [x] 3.5 Add a compile-time canary: confirm `physics`/`tests` build with NO GLFW/OpenGL include paths (a deliberate include of `<glad/glad.h>` in `physics` must fail)

## 4. Platform layer (window + GL context)

- [x] 4.1 Implement `platform::Window` (RAII): GLFW init, window create with GL 3.3 Core profile hints, make context current, load glad; clear diagnostics + non-zero exit on failure
- [x] 4.2 Implement the run loop: poll events, invoke a per-frame render callback, swap buffers, exit on window-close or Escape; clean shutdown (terminate GLFW, zero exit code)
- [x] 4.3 Log GL version, renderer string, and GLSL version at startup
- [x] 4.4 Add a GL-error check helper used after the smoke-test draw

## 5. Triangle smoke test

- [x] 5.1 In `app/main.cpp`, construct a `Window`, clear framebuffer to a known color
- [x] 5.2 Compile a minimal vertex+fragment shader, upload a single-triangle VBO/VAO, issue the draw call in the render callback
- [x] 5.3 Confirm no GL errors are reported for the frame; triangle is visible

## 6. Tests and CI

- [x] 6.1 Create `tests/` target linking doctest + `physics` only (never `platform`/GLFW); add one trivial passing test; register with CTest
- [x] 6.2 Verify `ctest` runs headlessly and passes on both platforms <!-- passes on Windows (MSVC) and Linux (GCC) -->
- [x] 6.3 Add `.github/workflows/ci.yml`: matrix (windows-msvc, ubuntu-gcc, ubuntu-clang) running configure + build (warnings-as-errors) + ctest; install Linux GL deps (e.g. `libgl1-mesa-dev`, `xorg-dev`)

## 7. Cross-platform verification and docs

- [x] 7.1 Build and run on Linux/WSL2 (WSLg): window opens, GL info logged, triangle renders; record observed GL version/renderer <!-- GL 4.5 (Core) Mesa 25.2.8, llvmpipe, GLSL 4.50; clean exit 0 -->
- [x] 7.2 Build and run on Windows (MSVC): window opens, GL info logged, triangle renders <!-- GL 3.3.0, Intel Arc Pro Graphics, GLSL 3.30; clean exit 0 -->
- [x] 7.3 If WSLg lacks hardware GL 3.3, validate and document the software-GL fallback (`LIBGL_ALWAYS_SOFTWARE=1` / required mesa packages) <!-- WSLg defaults to llvmpipe (software); LIBGL_ALWAYS_SOFTWARE=1 verified identical; documented in README -->
- [x] 7.4 Write `README.md`: prerequisites, the two-command build/run for each platform, the WSLg fallback note
- [x] 7.5 Confirm CI is green on all three matrix legs <!-- run 27570458069: windows-msvc, ubuntu-gcc, ubuntu-clang all green -->

## 8. Wrap-up

- [x] 8.1 Run `openspec validate phase-0-skeleton` and resolve any issues
- [x] 8.2 Update `docs/design/DESIGN.md` §13 to mark the WSLg/OpenGL risk resolved (with the observed GL info)
