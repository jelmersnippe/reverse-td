#include "systems/scene_manager.hpp"
#include "core/input.hpp"
#include "globals.hpp"
#include "raylib.h"
#include <cassert>

void SceneManager::SetScene(GameState& state, const Scene& scene) {
    Clear(state);
    PushScene(state, scene);
}

void SceneManager::PushScene(GameState& state, const Scene& scene) {
    scenes.push(scene);

    if (scene.init != nullptr) scene.init(state);
}

void SceneManager::PopScene(GameState& state) {
    if (scenes.empty()) return;

    Scene scene_to_remove = scenes.top();
    if (scene_to_remove.destroy != nullptr) scene_to_remove.destroy(state);

    scenes.pop();
}

void SceneManager::Update(GameState& state) {
    assert(!scenes.empty() && "No scene available to update");

    Scene current_scene = scenes.top();

    if (current_scene.update != nullptr) current_scene.update(state);

    BeginDrawing();

    if (current_scene.draw != nullptr) current_scene.draw(state);

    render_sprite({"crosshair", {.x = DEFAULT_SPRITE_SIZE, .y = DEFAULT_SPRITE_SIZE}, {.x = 2, .y = 2}},
                  input_frame.state.mouse_position);

    EndDrawing();
}

void SceneManager::Clear(GameState& state) {
    while (!scenes.empty()) {
        PopScene(state);
    }
}

SceneManager SCENE_MANAGER = {};
