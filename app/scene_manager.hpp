#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "scene.hpp"

// Owns the registered scenes and the gallery (DESIGN §7). Draws a selectable list of scenes
// as UI buttons and switches the active scene at runtime.

namespace app {

class SceneManager {
public:
    void add(std::unique_ptr<Scene> scene);
    Scene* active() const;
    void select(std::size_t index);  // makes it active and resets it to initial conditions

    // Draw the gallery as a list of buttons; clicking one switches to that scene.
    void draw_gallery();

private:
    std::vector<std::unique_ptr<Scene>> scenes_;
    std::size_t active_index_ = 0;
};

}  // namespace app
