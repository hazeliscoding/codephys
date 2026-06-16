// CodePhys — Phase 1 application: fixed-timestep loop, time control, and the scene gallery
// (DESIGN §7.1). Replaces the Phase 0 triangle smoke test.

#include <algorithm>
#include <cstdio>
#include <exception>
#include <memory>

#include <platform/window.hpp>
#include <render/color.hpp>
#include <render/renderer.hpp>
#include <ui/ui.hpp>

#include "scene_manager.hpp"
#include "scenes/ch03_projectile.hpp"
#include "scenes/integrator_comparison.hpp"

int main() {
    try {
        platform::Window window(1280, 800, "CodePhys - Phase 1");

        const platform::GlInfo info = platform::query_gl_info();
        std::printf("OpenGL version : %s\n", info.version.c_str());
        std::printf("Renderer       : %s\n", info.renderer.c_str());
        std::printf("GLSL version   : %s\n", info.glsl_version.c_str());

        render::Renderer renderer;

        app::SceneManager manager;
        manager.add(std::make_unique<app::ProjectileScene>());
        manager.add(std::make_unique<app::IntegratorComparisonScene>());
        manager.select(0);

        // Time control (DESIGN §7.1).
        bool playing = true;
        bool step_request = false;
        double time_scale = 1.0;
        double dt = 1.0 / 240.0;  // physics timestep (s)
        double accumulator = 0.0;
        double last_time = platform::now_seconds();

        bool prev_mouse_down = false;
        bool prev_space = false;
        bool reported = false;

        window.run([&](int fb_w, int fb_h) {
            // ---- Build the input snapshot from platform (keeps GLFW out of ui/scenes). ----
            ui::InputState input;
            double mx = 0.0, my = 0.0;
            window.mouse_position(mx, my);
            input.mouse = {mx, my};
            input.mouse_down = window.mouse_left_down();
            input.mouse_pressed = input.mouse_down && !prev_mouse_down;
            input.mouse_released = !input.mouse_down && prev_mouse_down;
            prev_mouse_down = input.mouse_down;
            input.scroll = window.take_scroll();

            const bool space = window.key_down(platform::keys::space);
            const bool space_pressed = space && !prev_space;
            prev_space = space;
            input.key_down[platform::keys::space] = space;
            input.key_pressed[platform::keys::space] = space_pressed;
            if (space_pressed) {
                playing = !playing;  // space toggles play/pause
            }

            // ---- Fixed-timestep stepping (DESIGN §7.1). ----
            const double now = platform::now_seconds();
            const double frame = std::min(now - last_time, 0.25);  // clamp catch-up spiral
            last_time = now;

            app::Scene* scene = manager.active();
            double alpha = 0.0;
            if (scene != nullptr) {
                if (playing) {
                    accumulator += frame * time_scale;
                    while (accumulator >= dt) {
                        scene->update(dt);
                        accumulator -= dt;
                    }
                    alpha = accumulator / dt;
                } else if (step_request) {
                    scene->update(dt);  // advance exactly one tick
                    step_request = false;
                }
            }

            // ---- World render pass (scene's camera). ----
            renderer.clear(render::Color{0.07f, 0.08f, 0.11f, 1.0f});
            if (scene != nullptr) {
                renderer.begin(scene->camera(fb_w, fb_h));
                scene->render(renderer, alpha);
                renderer.end();
            }

            // ---- UI pass (screen-space). ----
            render::Camera2D ui_cam;
            ui_cam.viewport_width = fb_w;
            ui_cam.viewport_height = fb_h;
            renderer.begin(ui_cam);

            constexpr double kPanelWidth = 400.0;
            ui::begin(renderer, input, {16.0, 16.0}, kPanelWidth);
            ui::heading("CodePhys");
            ui::help("Pick a scene, press Play (or Space), and drag the sliders. Esc quits.");
            ui::separator();
            manager.draw_gallery();
            ui::separator();
            ui::label(playing ? "State: Playing" : "State: Paused");
            if (ui::button(playing ? "Pause" : "Play")) {
                playing = !playing;
            }
            if (ui::button("Step (1 frame)")) {
                step_request = true;
            }
            if (ui::button("Reset") && scene != nullptr) {
                scene->reset();
            }
            ui::sliderFloat("time scale", &time_scale, 0.1, 4.0);
            ui::sliderFloat("dt (s)", &dt, 0.001, 0.02);
            ui::end();

            if (scene != nullptr) {
                // Dock the scene panel flush to the right edge.
                const double right_x = static_cast<double>(fb_w) - kPanelWidth;
                ui::begin(renderer, input, {right_x, 12.0}, kPanelWidth);
                scene->ui(input);
                ui::end();
            }

            renderer.end();

            if (!reported) {
                reported = true;
                if (platform::check_gl_errors("first frame")) {
                    std::printf("Phase 1: scene gallery rendered, no GL errors.\n");
                }
            }
        });

        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::fprintf(stderr, "Fatal: %s\n", e.what());
        return EXIT_FAILURE;
    }
}
