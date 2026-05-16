#include "core/entity_pool.hpp"
#include "game_state.hpp"
#include "raylib.h"
#include "scenes/game_scene.hpp"
#include "systems/scene_manager.hpp"
#include "systems/targeting.hpp"

#include "globals.hpp"
#include "systems/enemy_system.hpp"
#include "systems/projectile_system.hpp"
#include "systems/spawner_system.hpp"
#include "systems/tower_system.hpp"

void HandleInput(GameState& state) {
    Vector2 player_direction = {.x = 0, .y = 0};

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
    InitAudioDevice();

    Player player = {.position = Vector2{.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2},
                     .health = {.max = PLAYER_STARTING_HEALTH, .current = PLAYER_STARTING_HEALTH}};
    GameState state = {.player = player};

    state.camera.target = {player.position};
    state.camera.offset = {.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2};
    state.camera.rotation = 0.0f;
    state.camera.zoom = 1.0f;

    SetTargetFPS(TARGET_FPS);

    CreateEntity(state.spawners, {.position = {.x = -200, .y = 200},
                                  .health = {.max = 20, .current = 20},
                                  .spawn_amount = 2,
                                  .initial_spawn = 2});
    CreateEntity(state.spawners, {.position = {.x = 1200, .y = -400}, .health = {.max = 20, .current = 20}});
    CreateEntity(state.spawners,
                 {.position = {.x = 600, .y = 800}, .health = {.max = 20, .current = 20}, .initial_spawn = 2});
    CreateEntity(state.spawners,
                 {.position = {.x = 0, .y = 1200}, .health = {.max = 20, .current = 20}, .initial_spawn = 1});

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
