# build-system Specification

## Purpose
Defines the cross-platform CMake build, dependency policy, module layering, test
harness, and continuous integration for CodePhys. It enforces the project's hard rules
(one runtime dependency, `physics/` isolation, identical Windows/Linux builds) at the
build-system level.

## Requirements

### Requirement: Cross-platform CMake configuration via presets
The build SHALL be driven entirely by CMake (>= 3.21) and SHALL provide
`CMakePresets.json` with at least the presets `windows-msvc`, `linux-gcc`, and
`linux-clang`. A developer with only a supported compiler and CMake installed SHALL be
able to configure and build with two commands and no manual dependency installation
beyond GLFW's system prerequisites.

#### Scenario: Configure and build on Linux
- **WHEN** a developer runs `cmake --preset linux-gcc` then `cmake --build --preset linux-gcc`
- **THEN** configuration and build complete successfully with no errors
- **AND** the `codephys` application binary and the test binary are produced

#### Scenario: Configure and build on Windows
- **WHEN** a developer runs `cmake --preset windows-msvc` then `cmake --build --preset windows-msvc --config RelWithDebInfo`
- **THEN** configuration and build complete successfully with no errors
- **AND** the `codephys.exe` application binary and the test binary are produced

### Requirement: C++20 with warnings-as-errors
All first-party targets SHALL compile as C++20 with a strict warning set treated as
errors (MSVC `/W4 /WX`; GCC/Clang `-Wall -Wextra -Wpedantic -Werror`). Third-party
vendored/fetched code SHALL be excluded from warnings-as-errors.

#### Scenario: A warning fails the build
- **WHEN** first-party code introduces a compiler warning
- **THEN** the build fails

#### Scenario: Third-party code does not fail the build
- **WHEN** GLFW, glad, or doctest emit warnings during their compilation
- **THEN** the first-party build is not failed by those warnings

### Requirement: Dependency acquisition honoring the one-runtime-dep rule
GLFW 3.4 SHALL be acquired via CMake `FetchContent` pinned to a specific tag/commit.
glad (generated GL loader) and doctest SHALL be vendored under `third_party/` and used
at build/test time only. No runtime dependency other than GLFW SHALL be introduced.

#### Scenario: GLFW is fetched reproducibly
- **WHEN** the project is configured on a clean checkout with network access
- **THEN** GLFW is fetched at the pinned version and linked into the platform target

#### Scenario: No undeclared runtime dependency
- **WHEN** the dependency graph of the `codephys` binary is inspected
- **THEN** GLFW is the only third-party runtime library it depends on

### Requirement: Module skeleton with enforced layering
The build SHALL define the module targets `physics`, `render`, `ui`, `platform`, and
`app` per DESIGN §5, with link visibility that enforces the dependency direction
`app -> {physics, render, ui}`, `render -> physics::math`, `ui -> render`,
`platform -> GLFW`. The `physics` target SHALL NOT link or include GLFW, OpenGL, glad,
or any rendering/UI/platform code.

#### Scenario: physics target stays stdlib-only
- **WHEN** the `physics` target is built
- **THEN** it links no GLFW/OpenGL/glad symbols and includes no rendering/UI/platform headers

#### Scenario: Layering violation is prevented
- **WHEN** `physics` code attempts to include a GLFW or OpenGL header
- **THEN** the build fails (the include path / link visibility does not expose them to `physics`)

### Requirement: Test harness with doctest and CTest
The build SHALL define a `tests` target using vendored doctest, registered with CTest,
runnable headlessly (no window/GPU) on both platforms. At least one trivial passing test
SHALL exist so the harness is exercised from day one.

#### Scenario: Tests run via ctest
- **WHEN** a developer runs `ctest` from the build directory on either platform
- **THEN** the test suite executes headlessly and reports all tests passing

### Requirement: Continuous integration matrix
The repository SHALL provide a GitHub Actions workflow that, on push and pull request,
configures, builds (warnings-as-errors), and runs the test suite across a matrix of
`windows-msvc`, `ubuntu-gcc`, and `ubuntu-clang`.

#### Scenario: CI validates all three matrix legs
- **WHEN** a commit is pushed or a pull request is opened
- **THEN** CI runs configure + build + test on windows-msvc, ubuntu-gcc, and ubuntu-clang
- **AND** any build warning or test failure fails the corresponding leg
