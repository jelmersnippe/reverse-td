#include "game_state.hpp"
#include "raylib.h"
#include "scenes/main_menu_scene.hpp"
#include "systems/scene_manager.hpp"
#include "systems/targeting.hpp"

#include "globals.hpp"
#include "systems/enemy_system.hpp"
#include "systems/projectile_system.hpp"
#include "systems/spawner_system.hpp"
#include "systems/tower_system.hpp"

void HandleInput(GameState& state) {
    if (IsKeyDown(KEY_A)) { state.inputs.push_back(Input::A); }
    if (IsKeyDown(KEY_D)) { state.inputs.push_back(Input::D); }
    if (IsKeyDown(KEY_W)) { state.inputs.push_back(Input::W); }
    if (IsKeyDown(KEY_S)) { state.inputs.push_back(Input::S); }

    if (IsKeyDown(KEY_X)) { state.inputs.push_back(Input::X); }

    if (IsKeyDown(KEY_ONE)) { state.inputs.push_back(Input::One); }
    if (IsKeyDown(KEY_TWO)) { state.inputs.push_back(Input::Two); }
    if (IsKeyDown(KEY_THREE)) { state.inputs.push_back(Input::Three); }
    if (IsKeyDown(KEY_FOUR)) { state.inputs.push_back(Input::Four); }

    if (IsKeyPressed(KEY_ESCAPE)) { state.inputs.push_back(Input::Escape); }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) { state.inputs.push_back(Input::LeftMouse); }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) { state.inputs.push_back(Input::RightMouse); }
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Reverse Tiddy");
    SetExitKey(KEY_NULL);
    SetTargetFPS(TARGET_FPS);
    InitAudioDevice();

    GameState state = {};

    SCENE_MANAGER.PushScene(state, MAIN_MENU_SCENE);

    while (!state.should_exit && !WindowShouldClose()) {
        HandleInput(state);

        SCENE_MANAGER.Update(state);
    }

    SCENE_MANAGER.Clear(state);

    CloseAudioDevice();

    CloseWindow();

    return 0;
}
