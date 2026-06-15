# platform-window Specification

## Purpose
Defines the platform layer's responsibilities: opening a window with an OpenGL 3.3 Core
context via GLFW, running a clean event loop, reporting GL/driver information, and proving
the end-to-end GL pipeline with a triangle smoke test — identically on Windows (MSVC) and
Linux/WSL2 (WSLg).

## Requirements

### Requirement: Window with an OpenGL 3.3 Core context
The platform layer SHALL open a window and create an OpenGL 3.3 Core profile context
using GLFW, loading GL function pointers via glad. The window SHALL have a configurable
title and initial size, and SHALL support being closed by the user (window close button
or Escape key).

#### Scenario: Window opens with a valid GL 3.3 context
- **WHEN** the application starts on a supported platform
- **THEN** a window appears
- **AND** an OpenGL context reporting version >= 3.3 Core is current on the render thread

#### Scenario: User closes the window
- **WHEN** the user clicks the window close button or presses Escape
- **THEN** the event loop exits and the process shuts down cleanly with no leaked GLFW/GL resources and a zero exit code

### Requirement: Event loop with clean lifecycle
The platform layer SHALL run a loop that polls input events, lets the application render
a frame, and swaps buffers each iteration, until a close is requested. GLFW
initialization failures and context-creation failures SHALL be reported with a clear
diagnostic message and a non-zero exit code rather than crashing.

#### Scenario: Frames are presented continuously
- **WHEN** the application is running and no close is requested
- **THEN** the loop polls events, renders, and swaps buffers each iteration without busy-erroring

#### Scenario: Initialization failure is reported
- **WHEN** GLFW or context creation fails (e.g., no usable GL driver)
- **THEN** the application prints a clear error describing the failure and exits non-zero without crashing

### Requirement: Report GL and driver information
On startup the application SHALL log the OpenGL version, renderer (GPU/driver) string,
and GLSL version. This makes cross-platform and WSLg/software-GL differences visible for
debugging.

#### Scenario: GL info is logged at startup
- **WHEN** the application starts
- **THEN** it prints the GL version, renderer string, and GLSL version to standard output

### Requirement: Triangle smoke-test render
The application SHALL clear the framebuffer to a known color and draw a single triangle
using a minimal shader program and vertex buffer, proving the end-to-end GL pipeline
(context, shader compilation, buffer upload, draw call) functions. This render is a
throwaway smoke test, not the Phase 1 renderer.

#### Scenario: Triangle is rendered
- **WHEN** the application runs a frame
- **THEN** the framebuffer is cleared to the known color and a single triangle is visible
- **AND** no OpenGL errors are reported for the frame

### Requirement: Cross-platform parity (Windows and Linux/WSL2)
The window, context, event loop, and triangle smoke test SHALL behave identically on
Windows (MSVC) and Linux/WSL2 under WSLg. Where WSLg provides only a software or virtual
GL implementation, the application SHALL still run; a documented fallback (e.g.
requesting compatibility hints or `LIBGL_ALWAYS_SOFTWARE`) SHALL be available in the
README if hardware acceleration is unavailable.

#### Scenario: Identical behavior on both platforms
- **WHEN** the application is built and run on Windows and on Linux/WSL2 (WSLg)
- **THEN** both open a window, log GL info, and render the triangle with equivalent visible results

#### Scenario: Software-GL fallback is documented
- **WHEN** WSLg cannot provide a hardware-accelerated GL 3.3 context
- **THEN** the README documents a fallback that lets the application still run
