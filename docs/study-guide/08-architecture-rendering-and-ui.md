# 08 · Architecture, Rendering & UI

[← The Simulation Loop & Time](07-the-simulation-loop-and-time.md) · [Index](README.md)

---

> This section is about the *engine*, not the physics. It explains how CodePhys is wired so
> that the physics can stay pure and testable — useful background if you want to extend it.

## 🎯 Learning objectives

- Describe the **layered architecture** and the one-way dependency rule.
- Explain how the **2D renderer** turns shapes into batched OpenGL draws.
- Explain the **immediate-mode UI** model and how it avoids depending on the windowing system.
- Know where the **platform** boundary (GLFW/OpenGL) lives.

## 📖 Textbook map

None — this is software design. The relevant project docs are
[`docs/design/DESIGN.md`](../design/DESIGN.md) §5–§7 and [`CLAUDE.md`](../../CLAUDE.md).

---

## 🧱 The layered architecture

Dependencies point **downward only**. This is enforced physically by CMake target
visibility, not just by convention.

```text
            ┌─────────────────────────────┐
            │  app  (loop, scenes, gallery)│
            └───────────────┬─────────────┘
            ┌───────┬────────┼────────┬─────────┐
            ▼       ▼        ▼        ▼         ▼
          physics render    ui   platform   (tests)
            │       │        │        │
            │       └──▶ physics::math │
            │       ui ──▶ render      │
            ▼                          ▼
     C++ stdlib ONLY            GLFW + glad (OpenGL)
```

| Layer | Depends on | Must **not** touch |
|-------|-----------|--------------------|
| `physics/` | C++ stdlib only | GLFW, OpenGL, render, ui |
| `render/` | `physics::math`, OpenGL (glad) | ui, platform, physics core |
| `ui/` | `render` | GLFW/platform |
| `platform/` | GLFW + glad | physics, render, ui |
| `app/` | all of the above | — |

> [!IMPORTANT]
> The crown-jewel rule: **`physics/` has zero non-stdlib includes.** That isolation is what
> makes the entire physics library headlessly testable (no window, no GPU) and reusable in any
> renderer. The Phase 0 build even includes a "canary" proving a GL header *fails* to compile
> inside `physics/`.

There is exactly **one runtime dependency: GLFW**. `glad` (the GL loader) and `doctest` (tests)
are vendored, build-time only.

---

## 🎨 The 2D renderer

[`render/`](../../render/) is a small immediate-style API over OpenGL 3.3. Scenes call
high-level methods; all GL state stays inside the renderer (the public header exposes **no GL
types**).

```cpp
// render/renderer.hpp (excerpt)
void begin(const Camera2D& camera);                       // set view, clear batches
void line2D(Vec2 a, Vec2 b, Color, double width_px);
void circle(Vec2 center, double radius, Color, bool filled);
void polygon(std::span<const Vec2>, Color, bool filled);
void arrow2D(Vec2 from, Vec2 to, Color, double width_px);  // velocity/force vectors
void text(Vec2 screen_pos, std::string_view, Color, float scale);
void end();                                               // upload + draw everything
```

How it works:

- **Batching.** Primitives are decomposed into triangles and appended to a CPU vertex buffer;
  `end()` uploads the whole batch once and issues a couple of draw calls. Far cheaper than one
  draw per shape.
- **Two tiny GLSL programs.** A flat-color program for shapes, and a textured program for text.
- **`Camera2D`.** A pan/zoom orthographic camera ([`camera2d.hpp`](../../render/include/render/camera2d.hpp))
  that builds a view-projection matrix and offers `world_to_screen` / `screen_to_world`
  (used to drop world-anchored labels like `range` and `velocity`).
- **Text.** A vendored public-domain **8×8 bitmap font** uploaded once as an `R8` texture
  atlas; glyphs are textured quads. (Typography was deliberately deferred from Phase 0 to here.)

---

## 🖱️ The immediate-mode UI

[`ui/`](../../ui/) re-declares the whole panel **every frame** — there's no retained widget
tree. Scene control panels read like a script:

```cpp
ui::heading("Projectile");
ui::help("A ball thrown at an angle; gravity makes a parabola.");
changed |= ui::sliderFloat("speed (m/s)", &v0_mps_, 5.0, 60.0);
if (ui::button("Fire")) changed = true;
ui::plotLine("Energy (J)", energy_history_);
```

Two design points worth understanding:

- **Widgets edit caller state directly.** `sliderFloat` takes a `double*`; `button` returns
  `true` on the click frame. No callbacks, no event wiring.
- **Input is injected, not pulled.** To keep `ui` from depending on GLFW, the app builds a
  plain [`ui::InputState`](../../ui/include/ui/input.hpp) snapshot each frame (mouse, buttons,
  keys, scroll) from `platform` and passes it in. So `ui → render` only; the windowing system
  never leaks upward.

```text
platform (GLFW) ──reads──▶ app builds InputState ──passes──▶ ui / scenes
```

---

## 🪟 The platform boundary

[`platform/`](../../platform/) is the *only* place GLFW and OpenGL headers appear. It owns:

- `Window` — RAII over GLFW init, an OpenGL 3.3 Core context, and the glad loader.
- The event loop (`run`) that polls input, calls the per-frame callback, and swaps buffers.
- Input queries, a monotonic time source (`now_seconds`), and a GL-error check.

The native window handle is kept as an opaque `void*` in the public header, so even
`platform`'s *interface* doesn't leak GLFW types to `app`.

---

## 🔌 Extending the engine — a worked path

Want to add a new scene (say, a mass on a spring)? You only touch the top layer:

1. Create `app/scenes/your_scene.{hpp,cpp}` implementing the `Scene` interface
   ([`app/scene.hpp`](../../app/scene.hpp)): `reset / update / camera / render / ui / name`.
2. In `reset()`, configure a `World`: `set_acceleration(...)` with your force law,
   optionally `set_potential_energy(...)`, and `add(...)` particles.
3. Draw with the `Renderer` in `render()`; expose controls with `ui::` widgets in `ui()`.
4. Register it in `app/main.cpp` via `manager.add(std::make_unique<YourScene>())`.
5. Add a headless test in `tests/` asserting a closed-form result.

Notice what you **didn't** touch: `physics/math`, the integrators, the renderer, the loop. New
physics is new force functions + a scene — the composability the architecture is built for.

---

## ✅ Check yourself

<details>
<summary>1. Why can't <code>physics/</code> include an OpenGL header?</summary>

To keep it **renderer-agnostic and headlessly testable**. With zero GPU/windowing
dependencies, the physics tests run anywhere (CI with no display), and the library could be
reused under a completely different renderer. The build enforces this with a compile "canary."
</details>

<details>
<summary>2. What does "immediate-mode" UI mean here?</summary>

The UI is **re-declared every frame** from scratch — no persistent widget objects. A widget
both draws itself and returns its interaction result in the same call (`if (ui::button(...))`).
State lives in the caller, not the UI.
</details>

<details>
<summary>3. How does the UI react to the mouse without depending on GLFW?</summary>

The app reads raw input from `platform` (GLFW) and packs it into a plain `ui::InputState`
struct, which it passes into the UI each frame. The `ui` layer only ever sees that struct, so
it depends downward on `render` and never on the windowing system.
</details>

<details>
<summary>4. Where is the single allowed runtime dependency, and what's vendored instead?</summary>

The one runtime dependency is **GLFW**, used only inside `platform/`. `glad` (GL loader) and
`doctest` (tests) are **vendored** and build/test-time only — they ship nothing into the app
beyond what GLFW already requires.
</details>

---

[← The Simulation Loop & Time](07-the-simulation-loop-and-time.md) · [Index](README.md)
