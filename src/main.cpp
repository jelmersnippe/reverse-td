#include "core/entity_pool.hpp"
#include "game_state.hpp"
#include "raylib.h"
#include "scenes/game_scene.hpp"
#include "scenes/test_scene.hpp"
#include "systems/scene_manager.hpp"
#include "systems/targeting.hpp"

#include "globals.hpp"
#include "systems/enemy_system.hpp"
#include "systems/projectile_system.hpp"
#include "systems/spawner_system.hpp"
#include "systems/tower_system.hpp"

void HandleInput(GameState& state) {
    Vector2 player_direction = {.x = 0, .y = 0};
    if (IsKeyDown(KEY_F1)) { SCENE_MANAGER.SwapScene(state, TEST_SCENE); }

    if (IsKeyDown(KEY_A)) { player_direction.x -= 1; }
    if (IsKeyDown(KEY_D)) { player_direction.x += 1; }
    if (IsKeyDown(KEY_W)) { player_direction.y -= 1; }
    if (IsKeyDown(KEY_S)) { player_direction.y += 1; }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) { state.inputs.push_back(Input::FireWeapon); }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) { state.inputs.push_back(Input::DropTower); }

    state.player.direction = player_direction;
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Reverse Tiddy");
    SetTargetFPS(TARGET_FPS);
    InitAudioDevice();

    GameState state = {};

    SCENE_MANAGER.PushScene(state, GAME_SCENE);

    while (!state.should_exit && !WindowShouldClose()) {
        HandleInput(state);

        SCENE_MANAGER.Update(state);
    }

    SCENE_MANAGER.Dispose(state);

    CloseAudioDevice();

    CloseWindow();

    return 0;
}
