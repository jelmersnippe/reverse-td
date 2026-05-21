#include "scenes/test_scene.hpp"
#include "core/entity_pool.hpp"
#include "entities/enemy.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raylib.h"
#include "systems/enemy_system.hpp"
#include "systems/player_system.hpp"
#include "systems/projectile_system.hpp"
#include "systems/scene_manager.hpp"
#include "systems/spawner_system.hpp"
#include "systems/targeting.hpp"

#include "raymath.h"
#include "rlgl.h"
#include "systems/threat_director.hpp"
#include "systems/tower_system.hpp"
#include <format>

namespace {

const float CAMERA_SPEED = 200;

void Init(GameState& state) {
    state.camera.target = Vector2{.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2};

    CreateEntity(state.spawners, Spawner{.position = {.x = 200, .y = 600}});
    CreateEntity(state.spawners, Spawner{.position = {.x = -800, .y = 1000}});
    CreateEntity(state.spawners, Spawner{.position = {.x = -350, .y = -425}});
}

void Draw(const GameState& state) {
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

    for (Input input : state.inputs) {
        switch (input) {
            case Input::LeftMouse: {
                for (Slot<Enemy>& enemy_slot : state.enemies.data) {
                    enemy_slot.ref.state = EnemyState::Rally;
                }
                break;
            }
            case Input::One: {
                Enemy new_enemy = melee_enemy;
                new_enemy.position = destination;
                new_enemy.target_position = destination;
                new_enemy.damage = 0;
                CreateEntity(state.enemies, new_enemy);
                break;
            }
            case Input::Two: {
                Enemy new_enemy = fast_enemy;
                new_enemy.position = destination;
                new_enemy.target_position = destination;
                new_enemy.damage = 0;
                CreateEntity(state.enemies, new_enemy);
                break;
            }
            case Input::Three: {
                Enemy new_enemy = ranged_enemy;
                new_enemy.position = destination;
                new_enemy.target_position = destination;
                new_enemy.damage = 0;
                CreateEntity(state.enemies, new_enemy);
                break;
            }
            case Input::Four: {
                Enemy new_enemy = tank_enemy;
                new_enemy.position = destination;
                new_enemy.target_position = destination;
                new_enemy.damage = 0;
                CreateEntity(state.enemies, new_enemy);
                break;
            }
            case Input::W:
                state.camera.target += {.x = 0, .y = -camera_speed};
                break;
            case Input::S:
                state.camera.target += {.x = 0, .y = camera_speed};
                break;
            case Input::A:
                state.camera.target += {.x = -camera_speed, .y = 0};
                break;
            case Input::D:
                state.camera.target += {.x = camera_speed, .y = 0};
                break;
            default:
                break;
        }
    }

    state.inputs.clear();
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
