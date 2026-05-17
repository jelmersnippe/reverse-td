#include "scenes/game_scene.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raylib.h"
#include "systems/enemy_system.hpp"
#include "systems/projectile_system.hpp"
#include "systems/scene_manager.hpp"
#include "systems/spawner_system.hpp"
#include "systems/targeting.hpp"

#include "raymath.h"
#include "systems/tower_system.hpp"
#include <algorithm>
#include <format>
#include <iostream>

namespace {

void DrawHealth(const Vector2& position, const Health& health) {
    if (health.current >= health.max) return;

    const std::string health_text = std::format("{}/{}", health.current, health.max);
    const int text_width = MeasureText(health_text.c_str(), 12);

    DrawRectangle(position.x - 10, position.y, 20, 5, RED);
    DrawRectangle(position.x - 10, position.y, 20 * ((float)health.current / (float)health.max), 5, GREEN);
    DrawText(health_text.c_str(), position.x - text_width / 2, position.y, 12, BLACK);
}

void Draw(const GameState& state) {
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

    for (const Slot<Enemy>& enemy : state.enemies.data) {
        if (!enemy.alive) continue;

        const float radius = enemy.ref.size * ((float)enemy.ref.health.max / (float)BASE_ENEMY_HEALTH);
        DrawCircle(enemy.ref.position.x, enemy.ref.position.y, radius, RED);
        DrawHealth(enemy.ref.position - Vector2{.x = 0, .y = radius + 20}, enemy.ref.health);
    }

    for (const Slot<Spawner>& spawner : state.spawners.data) {
        if (!spawner.alive) continue;

        const Vector2 spawner_top_left = {.x = spawner.ref.position.x - SPAWNER_SIZE / 2,
                                          .y = spawner.ref.position.y - SPAWNER_SIZE / 2};
        DrawRectangleLines(spawner_top_left.x, spawner_top_left.y, SPAWNER_SIZE, SPAWNER_SIZE, BLACK);
        const int text_width = MeasureText("Spawner", 12);
        DrawText("Spawner", spawner.ref.position.x - text_width / 2, spawner.ref.position.y, 12, BLACK);

        DrawHealth(spawner.ref.position - Vector2{.x = 0, .y = SPAWNER_SIZE / 2 + 10}, spawner.ref.health);
    }

    for (const Slot<Tower>& tower : state.towers.data) {
        if (!tower.alive) continue;

        const Vector2 tower_top_left = {.x = tower.ref.position.x - TOWER_SIZE / 2,
                                        .y = tower.ref.position.y - TOWER_SIZE / 2};
        DrawRectangleLines(tower_top_left.x, tower_top_left.y, TOWER_SIZE, TOWER_SIZE, BLACK);
        DrawCircle(tower.ref.position.x, tower.ref.position.y, TOWER_SIZE * 0.3, BLUE);
        const int text_width = MeasureText("Tower", 12);
        DrawText("Tower", tower.ref.position.x - text_width / 2, tower.ref.position.y, 12, BLACK);

        DrawHealth(tower.ref.position - Vector2{.x = 0, .y = TOWER_SIZE / 2 + 10}, tower.ref.health);
    }

    EndMode2D();

    const std::string difficulty_text = std::format("Difficulty scale: {}", state.difficulty_scale);
    const int difficulty_text_width = MeasureText(difficulty_text.c_str(), 20);
    DrawText(difficulty_text.c_str(), SCREEN_WIDTH / 2 - difficulty_text_width / 2, 30, 20, BLACK);

    const std::string currency_text = std::format("Currency: {}", state.currency);
    const int currency_text_width = MeasureText(currency_text.c_str(), 20);
    DrawText(currency_text.c_str(), SCREEN_WIDTH / 2 - currency_text_width / 2, 60, 20, BLACK);
}

void UpdateInputs(GameState& state) {
    for (const Input& input : state.inputs) {
        switch (input) {
            case Input::LeftMouse: {
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
            case Input::RightMouse: {
                if (state.currency < TOWER_COST) break;

                const Vector2 destination = GetScreenToWorld2D(GetMousePosition(), state.camera);
                if (std::ranges::any_of(state.towers.data, [destination](const Slot<Tower>& tower_ref) {
                        return tower_ref.alive && CheckCollisionRecs({.x = destination.x - TOWER_SIZE / 2,
                                                                      .y = destination.y - TOWER_SIZE / 2,
                                                                      .width = TOWER_SIZE,
                                                                      .height = TOWER_SIZE},
                                                                     {.x = tower_ref.ref.position.x - TOWER_SIZE / 2,
                                                                      .y = tower_ref.ref.position.y - TOWER_SIZE / 2,
                                                                      .width = TOWER_SIZE,
                                                                      .height = TOWER_SIZE});
                    })) {
                    break;
                }
                if (std::ranges::any_of(state.spawners.data, [destination](const Slot<Spawner>& spawner_ref) {
                        return spawner_ref.alive &&
                               CheckCollisionRecs({.x = destination.x - TOWER_SIZE / 2,
                                                   .y = destination.y - TOWER_SIZE / 2,
                                                   .width = TOWER_SIZE,
                                                   .height = TOWER_SIZE},
                                                  {.x = spawner_ref.ref.position.x - SPAWNER_SIZE / 2,
                                                   .y = spawner_ref.ref.position.y - SPAWNER_SIZE / 2,
                                                   .width = SPAWNER_SIZE,
                                                   .height = SPAWNER_SIZE});
                    })) {
                    break;
                }
                if (std::ranges::any_of(state.enemies.data, [destination](const Slot<Enemy>& enemy_ref) {
                        return enemy_ref.alive &&
                               CheckCollisionPointRec(enemy_ref.ref.position, {.x = destination.x - TOWER_SIZE / 2,
                                                                               .y = destination.y - TOWER_SIZE / 2,
                                                                               .width = TOWER_SIZE,
                                                                               .height = TOWER_SIZE});
                    })) {
                    break;
                }

                CreateEntity(state.towers, {.position = destination, .health = {.max = 5, .current = 5}});
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

    Vector2 velocity = Vector2Normalize(state.player.direction) * PLAYER_SPEED * delta_time;

    Vector2 new_position = state.player.position;

    new_position.x += velocity.x;
    const CollisionResult collision_x = check_player_collision(state, new_position);

    new_position = state.player.position;
    new_position.y += velocity.y;
    const CollisionResult collision_y = check_player_collision(state, new_position);

    if (!collision_x.collided) state.player.position.x += velocity.x;
    if (!collision_y.collided) state.player.position.y += velocity.y;

    state.camera.target = {state.player.position};

    UpdateInputs(state);
    UpdateProjectiles(state);
    UpdateEnemies(state);
    UpdateSpawners(state);
    UpdateTowers(state);

    // TODO: Should show a death state / restart the game
    if (state.player.health.current <= 0) { state.should_exit = true; }
}

void Destroy(GameState& state) {
    state.Reset();
}

void Init(GameState& state) {
    Player player = {.position = Vector2{.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2},
                     .health = {.max = PLAYER_STARTING_HEALTH, .current = PLAYER_STARTING_HEALTH}};
    state.player = player;
    state.camera.target = {player.position};

    CreateEntity(state.spawners, {.position = {.x = -200, .y = 200},
                                  .health = {.max = 20, .current = 20},
                                  .spawn_amount = 2,
                                  .initial_spawn = 2});
    CreateEntity(state.spawners, {.position = {.x = 1200, .y = -400}, .health = {.max = 20, .current = 20}});
    CreateEntity(state.spawners,
                 {.position = {.x = 600, .y = 800}, .health = {.max = 20, .current = 20}, .initial_spawn = 2});
    CreateEntity(state.spawners,
                 {.position = {.x = 0, .y = 1200}, .health = {.max = 20, .current = 20}, .initial_spawn = 1});
}

} // namespace

const Scene GAME_SCENE = {.name = "Game", .init = Init, .update = Update, .draw = Draw, .destroy = Destroy};
