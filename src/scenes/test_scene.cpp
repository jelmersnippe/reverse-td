#include "scenes/test_scene.hpp"
#include "core/entity_pool.hpp"
#include "core/input.hpp"
#include "core/key.hpp"
#include "entities/enemy.hpp"
#include "entities/spawner.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "systems/enemy_system.hpp"
#include "systems/player_system.hpp"
#include "systems/projectile_system.hpp"
#include "systems/scene_manager.hpp"
#include "systems/spawner_system.hpp"

#include "raymath.h"
#include "rlgl.h"
#include "systems/threat_director.hpp"
#include "systems/tower_system.hpp"
#include <format>

namespace {

const float CAMERA_SPEED = 200;

void Init(GameState& state) {
    state.camera.target = Vector2{.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2};

    CreateEntity(state.spawners, Spawner{.position = {.x = 200, .y = 200}, .max_spawn = 2});
}

void Draw(GameState& state) {
    ClearBackground(WHITE);

    DrawText(std::format("Personal space: {}", PERSONAL_SPACE).c_str(), 20, 50, 12, BLACK);

    const std::string difficulty_text = std::format("Threat: {}", state.threat_director.threat);
    const int difficulty_text_width = MeasureText(difficulty_text.c_str(), 20);
    DrawText(difficulty_text.c_str(), SCREEN_WIDTH / 2 - difficulty_text_width / 2, 30, 20, BLACK);

    BeginMode2D(state.camera);
    rlPushMatrix();
    rlTranslatef(0, 25 * 50, 0);
    rlRotatef(90, 1, 0, 0);
    DrawGrid(1000, 50);
    rlPopMatrix();

    BeginMode2D(state.camera);

    DrawEnemies(state.enemies);
    DrawPlayers(state.players);
    DrawProjectiles(state.projectiles);
    DrawSpawners(state.spawners);
    DrawTowers(state.towers, state.camera);

    EndMode2D();
    DrawCircleV(GetMousePosition(), 4, DARKGRAY);

    EndMode2D();
}

void UpdateInputs(GameState& state) {
    const float delta_time = GetFrameTime();
    if (IsKeyDown(KEY_F5) && state.threat_director.threat > 0) state.threat_director.threat -= 0.01f;
    if (IsKeyDown(KEY_F6)) state.threat_director.threat += 0.01;

    state.camera.zoom = expf(logf(state.camera.zoom) + ((float)GetMouseWheelMove() * 0.1f));

    const Vector2 destination = GetScreenToWorld2D(GetMousePosition(), state.camera);

    const float camera_speed = (CAMERA_SPEED / state.camera.zoom) * delta_time;

    if (input_frame.is_mouse_pressed(Mouse::Left)) {
        for (Slot<Spawner>& spawner_slot : state.spawners.data) {
            spawner_slot.ref.state = SpawnerState::Rallying;
        }
    }
    if (input_frame.is_key_pressed(Key::Num1)) {
        Enemy new_enemy = melee_enemy;
        new_enemy.position = destination;
        new_enemy.target_position = destination;
        new_enemy.damage = 0;
        CreateEntity(state.enemies, new_enemy);
    }
    if (input_frame.is_key_pressed(Key::Num2)) {
        Enemy new_enemy = fast_enemy;
        new_enemy.position = destination;
        new_enemy.target_position = destination;
        new_enemy.damage = 0;
        CreateEntity(state.enemies, new_enemy);
    }
    if (input_frame.is_key_pressed(Key::Num3)) {
        Enemy new_enemy = ranged_enemy;
        new_enemy.position = destination;
        new_enemy.target_position = destination;
        new_enemy.damage = 0;
        CreateEntity(state.enemies, new_enemy);
    }
    if (input_frame.is_key_pressed(Key::Num4)) {
        Enemy new_enemy = tank_enemy;
        new_enemy.position = destination;
        new_enemy.target_position = destination;
        new_enemy.damage = 0;
        CreateEntity(state.enemies, new_enemy);
    }
    if (input_frame.is_key_down(Key::W)) state.camera.target += {.x = 0, .y = -camera_speed};
    if (input_frame.is_key_down(Key::S)) state.camera.target += {.x = 0, .y = camera_speed};
    if (input_frame.is_key_down(Key::A)) state.camera.target += {.x = -camera_speed, .y = 0};
    if (input_frame.is_key_down(Key::D)) state.camera.target += {.x = camera_speed, .y = 0};
}

void Update(GameState& state) {
    UpdateInputs(state);
    UpdatePlayers(state);
    UpdateTowers(state);
    UpdateProjectiles(state);
    UpdateEnemies(state);
    UpdateSpawners(state);
    UpdateThreatDirector(state);
}

void Destroy(GameState& state) {
    state.Reset();
}

} // namespace

const Scene TEST_SCENE = {.name = "Test", .init = Init, .update = Update, .draw = Draw, .destroy = Destroy};
