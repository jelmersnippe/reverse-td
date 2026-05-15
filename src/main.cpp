#include "core/gen_index.hpp"
#include "core/health.hpp"
#include "core/targeting.hpp"
#include "game_state.hpp"
#include "raylib.h"
#include "raymath.h"

#include <algorithm>
#include <format>
#include <iostream>

#include "globals.hpp"
#include "systems/enemy_system.hpp"
#include "systems/projectile_system.hpp"
#include "systems/spawner_system.hpp"
#include "systems/tower_system.hpp"

void DrawHealth(const Vector2& position, const Health& health) {
    if (health.current >= health.max) return;

    const std::string health_text = std::format("{}/{}", health.current, health.max);
    const int text_width = MeasureText(health_text.c_str(), 12);

    DrawRectangle(position.x - 10, position.y, 20, 5, RED);
    DrawRectangle(position.x - 10, position.y, 20 * ((float)health.current / (float)health.max), 5, GREEN);
    DrawText(health_text.c_str(), position.x - text_width / 2, position.y, 12, BLACK);
}

void UpdateInputs(GameState& state) {
    for (const Input& input : state.inputs) {
        switch (input) {
            case Input::FireWeapon: {
                if (state.player.time_since_last_shot < TIME_BETWEEN_SHOTS) break;
                std::cout << state.player.time_since_last_shot << std::endl;

                const Vector2 direction =
                    Vector2Subtract(GetScreenToWorld2D(GetMousePosition(), state.camera), state.player.position);
                CreateEntity(state.projectiles, {.velocity = Vector2Normalize(direction) * PROJECTILE_SPEED,
                                                 .position = state.player.position,
                                                 .life_time = 2.0});
                state.player.time_since_last_shot = 0;
                break;
            }
            case Input::DropTower: {
                if (state.currency < TOWER_COST) break;

                const Vector2 destination = GetScreenToWorld2D(GetMousePosition(), state.camera) -
                                            Vector2{.x = TOWER_SIZE / 2, .y = TOWER_SIZE / 2};
                if (std::ranges::any_of(state.towers.data, [destination](const Slot<Tower>& tower_ref) {
                        return tower_ref.alive &&
                               CheckCollisionRecs(
                                   {.x = destination.x, .y = destination.y, .width = TOWER_SIZE, .height = TOWER_SIZE},
                                   {.x = tower_ref.ref.position.x,
                                    .y = tower_ref.ref.position.y,
                                    .width = TOWER_SIZE,
                                    .height = TOWER_SIZE});
                    })) {
                    break;
                }

                CreateEntity(state.towers, {.position = destination});
                state.currency -= TOWER_COST;
                break;
            }
            default:
                throw "Unrecognized input";
        }
    }

    state.inputs.clear();
}

void Update(GameState& state) {
    const float delta_time = GetFrameTime();
    const std::vector<Targetable> targetables = build_targetables(state);

    state.player.time_since_last_shot += delta_time;

    const Vector2 normalized_direction = Vector2Normalize(state.player.direction);
    state.player.position += normalized_direction * PLAYER_SPEED * delta_time;

    state.camera.target = {state.player.position};

    UpdateInputs(state);
    UpdateProjectiles(state);
    UpdateEnemies(state);
    UpdateSpawners(state);
    UpdateTowers(state);

    // TODO: Should show a death state / restart the game
    if (state.player.health.current <= 0) { state.should_exit = true; }
};

void Draw(const GameState& state) {
    BeginDrawing();
    ClearBackground(WHITE);

    BeginMode2D(state.camera);

    DrawRectangle(state.player.position.x - PLAYER_SIZE / 2, state.player.position.y - PLAYER_SIZE / 2, PLAYER_SIZE,
                  PLAYER_SIZE, GREEN);
    DrawHealth(state.player.position - Vector2{.x = 0, .y = PLAYER_SIZE}, state.player.health);

    // TODO: Cull stuff outside of the screen
    for (const Slot<Projectile>& projectile : state.projectiles.data) {
        if (!projectile.alive) continue;

        DrawCircle(projectile.ref.position.x, projectile.ref.position.y, PROJECTILE_SIZE, ORANGE);
    }

    std::cout << "Drawing enemy count: " << state.enemies.data.size() << std::endl;
    for (const Slot<Enemy>& enemy : state.enemies.data) {
        if (!enemy.alive) continue;

        const float radius = ENEMY_SIZE * ((float)enemy.ref.health.max / (float)BASE_ENEMY_HEALTH);
        DrawCircle(enemy.ref.position.x, enemy.ref.position.y, radius, RED);
        DrawHealth(enemy.ref.position - Vector2{.x = 0, .y = radius + 20}, enemy.ref.health);
    }

    for (const Slot<Spawner>& spawner : state.spawners.data) {
        if (!spawner.alive) continue;

        DrawRectangleLines(spawner.ref.position.x, spawner.ref.position.y, SPAWNER_SIZE, SPAWNER_SIZE, BLACK);
        const int text_width = MeasureText("Spawner", 12);
        DrawText("Spawner", spawner.ref.position.x - text_width / 2 + SPAWNER_SIZE / 2,
                 spawner.ref.position.y + SPAWNER_SIZE / 2, 12, BLACK);

        DrawHealth(spawner.ref.position + Vector2{.x = SPAWNER_SIZE / 2.0, .y = 10}, spawner.ref.health);
    }

    for (const Slot<Tower>& tower : state.towers.data) {
        if (!tower.alive) continue;

        DrawRectangleLines(tower.ref.position.x, tower.ref.position.y, TOWER_SIZE, TOWER_SIZE, BLACK);
        DrawCircle(tower.ref.position.x + TOWER_SIZE / 2, tower.ref.position.y + TOWER_SIZE / 2, TOWER_SIZE * 0.3,
                   BLUE);
        const int text_width = MeasureText("Tower", 12);
        DrawText("Tower", tower.ref.position.x - text_width / 2 + TOWER_SIZE / 2, tower.ref.position.y + TOWER_SIZE / 2,
                 12, BLACK);

        DrawHealth(tower.ref.position + Vector2{.x = SPAWNER_SIZE / 2, .y = 10}, tower.ref.health);
    }

    EndMode2D();

    const std::string difficulty_text = std::format("Difficulty scale: {}", state.difficulty_scale);
    const int difficulty_text_width = MeasureText(difficulty_text.c_str(), 20);
    DrawText(difficulty_text.c_str(), SCREEN_WIDTH / 2 - difficulty_text_width / 2, 30, 20, BLACK);

    const std::string currency_text = std::format("Currency: {}", state.currency);
    const int currency_text_width = MeasureText(currency_text.c_str(), 20);
    DrawText(currency_text.c_str(), SCREEN_WIDTH / 2 - currency_text_width / 2, 60, 20, BLACK);

    EndDrawing();
}

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

    while (!state.should_exit && !WindowShouldClose()) {
        HandleInput(state);

        Update(state);

        Draw(state);
    }

    CloseAudioDevice();

    CloseWindow();

    return 0;
}
