#include "scene_manager.hpp"

#include <string>

#include <ui/ui.hpp>

namespace app {

void SceneManager::add(std::unique_ptr<Scene> scene) {
    scenes_.push_back(std::move(scene));
}

Scene* SceneManager::active() const {
    return scenes_.empty() ? nullptr : scenes_[active_index_].get();
}

void SceneManager::select(std::size_t index) {
    if (index < scenes_.size()) {
        active_index_ = index;
        scenes_[active_index_]->reset();
    }
}

void SceneManager::draw_gallery() {
    ui::label("Scenes:");
    for (std::size_t i = 0; i < scenes_.size(); ++i) {
        const bool is_active = (i == active_index_);
        std::string entry = (is_active ? "> " : "  ") + std::string(scenes_[i]->name());
        if (ui::button(entry) && !is_active) {
            select(i);
        }
    }
}

}  // namespace app
