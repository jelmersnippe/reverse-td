#include "raylib.h"
#include "raymath.h"

#include <algorithm>
#include <format>
#include <iostream>

#include "game_state.hpp"

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;
const int TARGET_FPS = 60;

const int PLAYER_STARTING_HEALTH = 10;
const int PLAYER_SPEED = 200;
const int PLAYER_SIZE = 50;

const int ENEMY_SPEED = 100;
const int ENEMY_SIZE = 20;
const int ENEMY_ATTACK_RANGE = 5;

const int SPAWNER_SIZE = 100;

const int PROJECTILE_SPEED = 1000;
const int PROJECTILE_SIZE = 5;

const float FIRE_RATE = 150;
const float TIME_BETWEEN_SHOTS = 60 / FIRE_RATE;

const int TOWER_SIZE = 100;

void DrawHealth(const Vector2& position, const Health& health) {
    const std::string health_text = std::format("{}/{}", health.current, health.max);
    const int text_width = MeasureText(health_text.c_str(), 12);

    DrawRectangle(position.x - 10, position.y, 20, 5, RED);
    DrawRectangle(position.x - 10, position.y, 20 * ((float)health.current / (float)health.max), 5, GREEN);
    DrawText(health_text.c_str(), position.x - text_width / 2, position.y, 12, BLACK);
}

void Update(GameState& state) {
    const float delta_time = GetFrameTime();

    state.time_since_last_shot += delta_time;

    const Vector2 normalized_direction = Vector2Normalize(state.player_direction);
    state.player_position += normalized_direction * PLAYER_SPEED * delta_time;

    state.camera.target = {state.player_position};

    for (const Input& input : state.inputs) {
        switch (input) {
            case Input::FireWeapon: {
                if (state.time_since_last_shot < TIME_BETWEEN_SHOTS) break;
                std::cout << state.time_since_last_shot << std::endl;

                state.projectiles.push_back(
                    {.velocity = Vector2Normalize(Vector2Subtract(GetScreenToWorld2D(GetMousePosition(), state.camera),
                                                                  state.player_position)) *
                                 PROJECTILE_SPEED,
                     .position = state.player_position,
                     .life_time = 2.0});
                state.time_since_last_shot = 0;
                break;
            }
            case Input::DropTower: {
                const Vector2 destination = GetScreenToWorld2D(GetMousePosition(), state.camera);
                if (std::ranges::any_of(state.towers.data, [destination](const Slot<Tower>& tower_ref) {
                        return tower_ref.alive && CheckCollisionPointRec(destination, {.x = tower_ref.ref.position.x,
                                                                                       .y = tower_ref.ref.position.y,
                                                                                       .width = TOWER_SIZE,
                                                                                       .height = TOWER_SIZE});
                    })) {
                    break;
                }

                CreateEntity(state.towers, {.position = destination});
                break;
            }
            default:
                throw "Unrecognized input";
        }
    }

    state.inputs.clear();

    std::vector<size_t> spawner_indexes_to_remove = {};
    std::vector<size_t> projectile_indexes_to_remove = {};
    for (size_t projectile_index = 0; projectile_index < state.projectiles.size(); projectile_index++) {
        bool hit = false;
        Projectile& projectile = state.projectiles[projectile_index];

        projectile.time_alive += delta_time;

        if (projectile.time_alive >= projectile.life_time) {
            projectile_indexes_to_remove.push_back(projectile_index);
            continue;
        }

        const Vector2 old_position = projectile.position;

        projectile.position += projectile.velocity * delta_time;

        for (size_t enemy_index = 0; enemy_index < state.enemies.data.size(); enemy_index++) {
            Slot<Enemy>& enemy = state.enemies.data[enemy_index];
            if (!enemy.alive) continue;

            // Projectile trajectory would collide with enemy.
            // This is done with a line instead of point because a projectile could move fast enough
            // to fully pass through an enemy in a frame, thus the point would not be in the circle ever
            // TODO: This only checks center point of projectile. Should also check radius. So cylender/rectangle for
            // path
            hit = CheckCollisionCircleLine(enemy.ref.position, ENEMY_SIZE, old_position, projectile.position);

            if (!hit) continue;

            enemy.ref.health.current -= projectile.damage;

            if (enemy.ref.health.current <= 0) {
                state.difficulty_scale += 0.05;
                DestroyEntity(state.enemies, {.index = enemy_index, .generation = enemy.generation});
            }

            projectile_indexes_to_remove.push_back(projectile_index);
            break;
        }

        if (hit) continue;

        for (size_t spawner_index = 0; spawner_index < state.spawners.size(); spawner_index++) {
            Spawner& spawner = state.spawners[spawner_index];

            hit = CheckCollisionPointRec(
                projectile.position,
                {.x = spawner.position.x, .y = spawner.position.y, .width = SPAWNER_SIZE, .height = SPAWNER_SIZE});

            if (!hit) continue;

            spawner.health.current -= projectile.damage;

            if (spawner.health.current <= 0) {
                spawner_indexes_to_remove.push_back(spawner_index);
                state.difficulty_scale += 0.2;
            }

            projectile_indexes_to_remove.push_back(projectile_index);
            break;
        }
    }

    for (const size_t index : projectile_indexes_to_remove) {
        state.projectiles.erase(state.projectiles.begin() + index);
    }

    for (const size_t index : spawner_indexes_to_remove) {
        state.spawners.erase(state.spawners.begin() + index);
    }

    for (size_t i = 0; i < state.enemies.data.size(); i++) {
        Slot<Enemy>& enemy = state.enemies.data[i];

        if (!enemy.alive) continue;

        enemy.ref.time_since_last_attack += delta_time;

        // TODO: Fix enemy converging
        if (Vector2Distance(state.player_position, enemy.ref.position) <=
            (PLAYER_SIZE / 2) + (ENEMY_SIZE / 2) + ENEMY_ATTACK_RANGE) {
            // If in range -> stand still and attack
            enemy.ref.velocity = {.x = 0, .y = 0};

            if (enemy.ref.time_since_last_attack >= enemy.ref.attack_cooldown) {
                state.player_health.current -= enemy.ref.damage;
                enemy.ref.time_since_last_attack = 0;
            }
        } else {
            // Else -> move closer
            enemy.ref.velocity = Vector2Normalize(state.player_position - enemy.ref.position) * ENEMY_SPEED;
            enemy.ref.position += enemy.ref.velocity * delta_time;
        }
    }

    for (Spawner& spawner : state.spawners) {
        if (!spawner.initial_spawn_happened) {
            for (int i = 0; i < spawner.initial_spawn; i++) {
                CreateEntity(state.enemies,
                             {.position = spawner.position + Vector2{.x = static_cast<float>(0.5 * i), .y = 0},
                              .health = {.max = static_cast<int>(3 * state.difficulty_scale),
                                         .current = static_cast<int>(3 * state.difficulty_scale)},
                              .damage = static_cast<int>(1 * state.difficulty_scale)});
            }

            spawner.initial_spawn_happened = true;
            continue;
        }

        spawner.time_since_last_spawn += delta_time;
        if (spawner.time_since_last_spawn >= (spawner.spawn_cooldown / state.difficulty_scale)) {
            const int spawn_count = static_cast<int>((float)spawner.spawn_amount * state.difficulty_scale);
            for (int i = 0; i < spawn_count; i++) {
                CreateEntity(state.enemies,
                             {.position = spawner.position + Vector2{.x = static_cast<float>(0.5 * i), .y = 0},
                              .health = {.max = static_cast<int>(3 * state.difficulty_scale),
                                         .current = static_cast<int>(3 * state.difficulty_scale)},
                              .damage = static_cast<int>(1 * state.difficulty_scale)});
            }

            spawner.time_since_last_spawn = 0;
        }
    }

    for (Slot<Tower>& tower : state.towers.data) {
        tower.ref.time_since_last_attack += delta_time;

        Enemy* target = GetEntity(state.enemies, tower.ref.target);

        // Check if target left range
        if (target != nullptr) {
            const float distance = Vector2Distance(tower.ref.position, target->position);

            if (distance > tower.ref.range) target = nullptr;
        }

        // Acquire target
        if (target == nullptr) {
            for (uint32_t i = 0; i < state.enemies.data.size(); i++) {
                Slot<Enemy>& enemy = state.enemies.data[i];
                if (!enemy.alive) continue;

                const float distance = Vector2Distance(tower.ref.position, enemy.ref.position);
                if (distance <= tower.ref.range) {
                    tower.ref.target = EntityHandle{.index = i, .generation = enemy.generation};
                    target = &enemy.ref;
                    break;
                }
            }
        }

        // Target acquiring failed
        if (target == nullptr) continue;

        // Attacking time
        if (tower.ref.time_since_last_attack >= 60.0 / tower.ref.fire_rate) {
            const Vector2 tower_center = tower.ref.position + Vector2{.x = TOWER_SIZE / 2, .y = TOWER_SIZE / 2};
            state.projectiles.push_back(
                {.velocity = Vector2Normalize(target->position - tower_center) * PROJECTILE_SPEED,
                 .position = tower_center,
                 .life_time = 2.0});
            tower.ref.time_since_last_attack = 0;
        }
    }

    // TODO: Should show a death state / restart the game
    if (state.player_health.current <= 0) { CloseWindow(); }
};

void Draw(const GameState& state) {
    BeginDrawing();
    ClearBackground(WHITE);

    BeginMode2D(state.camera);

    DrawRectangle(state.player_position.x - PLAYER_SIZE / 2, state.player_position.y - PLAYER_SIZE / 2, PLAYER_SIZE,
                  PLAYER_SIZE, GREEN);
    if (state.player_health.current < state.player_health.max) {
        DrawHealth(state.player_position - Vector2{.x = 0, .y = PLAYER_SIZE / 2 + 10}, state.player_health);
    }

    // TODO: Cull stuff outside of the screen
    for (const Projectile& projectile : state.projectiles) {
        DrawCircle(projectile.position.x, projectile.position.y, PROJECTILE_SIZE, ORANGE);
    }

    for (const Slot<Enemy>& enemy : state.enemies.data) {
        if (!enemy.alive) continue;

        DrawCircle(enemy.ref.position.x, enemy.ref.position.y, ENEMY_SIZE, RED);
        if (enemy.ref.health.current < enemy.ref.health.max) {
            DrawHealth(enemy.ref.position - Vector2{.x = 0, .y = ENEMY_SIZE / 2 + 10}, enemy.ref.health);
        }
    }

    for (const Spawner& spawner : state.spawners) {
        DrawRectangleLines(spawner.position.x, spawner.position.y, SPAWNER_SIZE, SPAWNER_SIZE, BLACK);
        const int text_width = MeasureText("Spawner", 12);
        DrawText("Spawner", spawner.position.x - text_width / 2 + SPAWNER_SIZE / 2,
                 spawner.position.y + SPAWNER_SIZE / 2, 12, BLACK);

        if (spawner.health.current < spawner.health.max) {
            DrawHealth(spawner.position + Vector2{.x = SPAWNER_SIZE / 2, .y = 10}, spawner.health);
        }
    }

    for (const Slot<Tower>& tower : state.towers.data) {
        DrawRectangleLines(tower.ref.position.x, tower.ref.position.y, TOWER_SIZE, TOWER_SIZE, BLACK);
        DrawCircle(tower.ref.position.x + TOWER_SIZE / 2, tower.ref.position.y + TOWER_SIZE / 2, TOWER_SIZE * 0.3,
                   BLUE);
        const int text_width = MeasureText("Tower", 12);
        DrawText("Tower", tower.ref.position.x - text_width / 2 + TOWER_SIZE / 2, tower.ref.position.y + TOWER_SIZE / 2,
                 12, BLACK);

        if (tower.ref.health.current < tower.ref.health.max) {
            DrawHealth(tower.ref.position + Vector2{.x = SPAWNER_SIZE / 2, .y = 10}, tower.ref.health);
        }
    }

    EndMode2D();

    const std::string difficulty_text = std::format("Difficulty scale: {}", state.difficulty_scale);
    const int difficulty_text_width = MeasureText(difficulty_text.c_str(), 20);
    DrawText(difficulty_text.c_str(), SCREEN_WIDTH / 2 - difficulty_text_width / 2, 30, 20, BLACK);

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

    state.player_direction = player_direction;
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Reverse Tiddy");
    InitAudioDevice();

    GameState state = {.player_position = Vector2{.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2},
                       .player_health = {.max = PLAYER_STARTING_HEALTH, .current = PLAYER_STARTING_HEALTH}};

    state.camera.target = {state.player_position};
    state.camera.offset = {.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2};
    state.camera.rotation = 0.0f;
    state.camera.zoom = 1.0f;

    SetTargetFPS(TARGET_FPS);

    state.spawners.push_back({.position = {.x = -200, .y = 200},
                              .health = {.max = 20, .current = 20},
                              .spawn_amount = 2,
                              .initial_spawn = 2});
    state.spawners.push_back({.position = {.x = 1200, .y = -400}, .health = {.max = 20, .current = 20}});
    state.spawners.push_back(
        {.position = {.x = 600, .y = 800}, .health = {.max = 20, .current = 20}, .initial_spawn = 2});
    state.spawners.push_back(
        {.position = {.x = 0, .y = 1200}, .health = {.max = 20, .current = 20}, .initial_spawn = 1});

    while (!WindowShouldClose()) {
        HandleInput(state);

        Update(state);

        Draw(state);
    }
    CloseAudioDevice();

    CloseWindow();

    return 0;
}
