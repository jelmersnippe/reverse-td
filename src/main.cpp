#include "raylib.h"
#include "raymath.h"

#include <format>
#include <iostream>

#include "game_state.hpp"

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
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

// Following camera (?)
// Dropping turrets
// Increasing difficulty

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

    for (const Input& input : state.inputs) {
        switch (input) {
            case Input::FireWeapon:
                if (state.time_since_last_shot < TIME_BETWEEN_SHOTS) continue;
                std::cout << state.time_since_last_shot << std::endl;

                state.projectiles.push_back(
                    {.velocity = Vector2Normalize(Vector2Subtract(GetMousePosition(), state.player_position)) *
                                 PROJECTILE_SPEED,
                     .position = state.player_position,
                     .life_time = 2.0});
                state.time_since_last_shot = 0;
                break;
            default:
                throw "Unrecognized input";
        }
    }

    state.inputs.clear();

    std::vector<size_t> enemy_indexes_to_remove = {};
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

        for (size_t enemy_index = 0; enemy_index < state.enemies.size(); enemy_index++) {
            Enemy& enemy = state.enemies[enemy_index];

            // Projectile trajectory would collide with enemy.
            // This is done with a line instead of point because a projectile could move fast enough
            // to fully pass through an enemy in a frame, thus the point would not be in the circle ever
            // TODO: This only checks center point of projectile. Should also check radius. So cylender/rectangle for
            // path
            hit = CheckCollisionCircleLine(enemy.position, ENEMY_SIZE, old_position, projectile.position);

            if (!hit) continue;

            enemy.health.current -= projectile.damage;

            if (enemy.health.current <= 0) { enemy_indexes_to_remove.push_back(enemy_index); }

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

            if (spawner.health.current <= 0) { spawner_indexes_to_remove.push_back(spawner_index); }

            projectile_indexes_to_remove.push_back(projectile_index);
            break;
        }
    }

    for (const size_t index : projectile_indexes_to_remove) {
        state.projectiles.erase(state.projectiles.begin() + index);
    }

    for (const size_t index : enemy_indexes_to_remove) {
        state.enemies.erase(state.enemies.begin() + index);
    }

    for (const size_t index : spawner_indexes_to_remove) {
        state.spawners.erase(state.spawners.begin() + index);
    }

    for (size_t i = 0; i < state.enemies.size(); i++) {
        Enemy& enemy = state.enemies[i];

        // TODO: Fix enemy converging
        if (Vector2Distance(state.player_position, enemy.position) <=
            (PLAYER_SIZE / 2) + (ENEMY_SIZE / 2) + ENEMY_ATTACK_RANGE) {
            // If in range -> stand still and attack
            enemy.velocity = {.x = 0, .y = 0};

            if (enemy.time_since_last_attack >= enemy.attack_cooldown) {
                state.player_health.current -= enemy.damage;
                enemy.time_since_last_attack = 0;
            }
        } else {
            // Else -> move closer
            enemy.velocity = Vector2Normalize(state.player_position - enemy.position) * ENEMY_SPEED;
            enemy.position += enemy.velocity * delta_time;
        }

        enemy.time_since_last_attack += delta_time;
    }

    for (Spawner& spawner : state.spawners) {
        if (!spawner.initial_spawn_happened) {
            for (int i = 0; i < spawner.initial_spawn; i++) {
                state.enemies.push_back(
                    {.position = spawner.position + Vector2{.x = static_cast<float>(30 * i), .y = 0},
                     .health = {.max = 3, .current = 3}});
            }

            spawner.initial_spawn_happened = true;
            continue;
        }

        if (spawner.time_since_last_spawn >= spawner.spawn_cooldown) {
            for (int i = 0; i < spawner.spawn_amount; i++) {
                state.enemies.push_back(
                    {.position = spawner.position + Vector2{.x = static_cast<float>(0.5 * i), .y = 0},
                     .health = {.max = 3, .current = 3}});
            }

            spawner.time_since_last_spawn = 0;
        }

        spawner.time_since_last_spawn += delta_time;
    }

    // TODO: Should show a death state / restart the game
    if (state.player_health.current <= 0) { CloseWindow(); }
};

void Draw(const GameState& state) {
    BeginDrawing();
    ClearBackground(WHITE);

    DrawRectangle(state.player_position.x - PLAYER_SIZE / 2, state.player_position.y - PLAYER_SIZE / 2, PLAYER_SIZE,
                  PLAYER_SIZE, GREEN);
    DrawHealth(state.player_position - Vector2{.x = 0, .y = PLAYER_SIZE / 2 + 10}, state.player_health);

    for (const Projectile& projectile : state.projectiles) {
        DrawCircle(projectile.position.x, projectile.position.y, PROJECTILE_SIZE, YELLOW);
    }

    for (const Enemy& enemy : state.enemies) {
        DrawCircle(enemy.position.x, enemy.position.y, ENEMY_SIZE, RED);
        DrawHealth(enemy.position - Vector2{.x = 0, .y = ENEMY_SIZE / 2 + 10}, enemy.health);
    }

    for (const Spawner& spawner : state.spawners) {
        DrawRectangleLines(spawner.position.x, spawner.position.y, SPAWNER_SIZE, SPAWNER_SIZE, BLACK);
        const int text_width = MeasureText("Spawner", 12);
        DrawText("Spawner", spawner.position.x - text_width / 2 + SPAWNER_SIZE / 2,
                 spawner.position.y + SPAWNER_SIZE / 2, 12, BLACK);

        DrawHealth(spawner.position + Vector2{.x = SPAWNER_SIZE / 2, .y = 10}, spawner.health);
    }

    EndDrawing();
}

void HandleInput(GameState& state) {
    Vector2 player_direction = {.x = 0, .y = 0};

    if (IsKeyDown(KEY_A)) { player_direction.x -= 1; }
    if (IsKeyDown(KEY_D)) { player_direction.x += 1; }
    if (IsKeyDown(KEY_W)) { player_direction.y -= 1; }
    if (IsKeyDown(KEY_S)) { player_direction.y += 1; }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) { state.inputs.push_back(Input::FireWeapon); }

    state.player_direction = player_direction;
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Reverse Tiddy");
    InitAudioDevice();
    SetTargetFPS(TARGET_FPS);

    GameState state = {.player_position = Vector2{.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2},
                       .player_health = {.max = PLAYER_STARTING_HEALTH, .current = PLAYER_STARTING_HEALTH}};

    state.spawners.push_back({.position = {.x = 200, .y = 200}, .health = {.max = 20, .current = 20}});

    while (!WindowShouldClose()) {
        HandleInput(state);

        Update(state);

        Draw(state);
    }
    CloseAudioDevice();

    CloseWindow();

    return 0;
}
