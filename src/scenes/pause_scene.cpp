#include "scenes/pause_scene.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raylib.h"
#include "systems/scene_manager.hpp"

namespace {

void Update(GameState& state) {
    for (Input& input : state.inputs) {
        switch (input) {
            case Input::Escape:
                if (SCENE_MANAGER.scenes.top().name == PAUSE_SCENE.name) { SCENE_MANAGER.PopScene(state); }
                break;
            default:
                break;
        }
    }

    state.inputs.clear();
}

void Draw(GameState& state) {
    ClearBackground(ColorAlpha(GRAY, 0.6));

    DrawText("Pause schermpie", SCREEN_CENTER.x, SCREEN_CENTER.y, 20, BLACK);
}
} // namespace

const Scene PAUSE_SCENE = {.name = "Pause", .init = nullptr, .update = Update, .draw = Draw, .destroy = nullptr};
