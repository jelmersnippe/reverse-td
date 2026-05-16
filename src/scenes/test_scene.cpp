#include "scenes/test_scene.hpp"
#include "core/entity_pool.hpp"
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
#include <format>

namespace {
static float PERSONAL_SPACE = 50;

void Init(GameState& state) {
    Player player = {.position = Vector2{.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2},
                     .health = {.max = PLAYER_STARTING_HEALTH, .current = PLAYER_STARTING_HEALTH}};
    state.player = player;
    state.camera.target = {player.position};
}

void Draw(const GameState& state) {
    ClearBackground(WHITE);

    DrawText(std::format("Personal space: {}", PERSONAL_SPACE).c_str(), 20, 50, 12, BLACK);

    BeginMode2D(state.camera);

    DrawRectangle(state.player.position.x - PLAYER_SIZE / 2, state.player.position.y - PLAYER_SIZE / 2, PLAYER_SIZE,
                  PLAYER_SIZE, GREEN);

    for (const Slot<Enemy>& enemy : state.enemies.data) {
        if (!enemy.alive) continue;

        const float radius = enemy.ref.size * ((float)enemy.ref.health.max / (float)BASE_ENEMY_HEALTH);
        DrawCircle(enemy.ref.position.x, enemy.ref.position.y, radius, RED);
    }

    EndMode2D();
}

void Update(GameState& state) {
    if (IsKeyDown(KEY_F5) && PERSONAL_SPACE > 0) PERSONAL_SPACE -= 1;
    if (IsKeyDown(KEY_F6)) PERSONAL_SPACE += 1;

    const float delta_time = GetFrameTime();
    Vector2 velocity = Vector2Normalize(state.player.direction) * PLAYER_SPEED * delta_time;
    state.player.position += velocity;

    state.camera.target = {state.player.position};

    for (Input input : state.inputs) {
        switch (input) {
            case Input::LeftMouse: {
                const Vector2 destination = GetScreenToWorld2D(GetMousePosition(), state.camera);
                Enemy new_enemy{};
                new_enemy.position = destination;
                CreateEntity(state.enemies, new_enemy);
                break;
            }
            default:
                break;
        }
    }

    state.inputs.clear();

    for (Slot<Enemy>& slot : state.enemies.data) {
        if (!slot.alive) continue;

        Enemy& enemy = slot.ref;

        Vector2 seek = Vector2Normalize(state.player.position - enemy.position);

        Vector2 separation{};
        for (Slot<Enemy>& other_slot : state.enemies.data) {
            if (!other_slot.alive) continue;
            const Enemy& other_enemy = other_slot.ref;

            const Vector2 direction_from_other = enemy.position - other_enemy.position;
            float distance = Vector2Length(direction_from_other);

            if (distance < 0.001f || distance > PERSONAL_SPACE) continue;

            float strength = 1.0f - (distance / PERSONAL_SPACE);

            separation += Vector2Normalize(direction_from_other) * strength;
        }

        Vector2 velocity = Vector2Normalize(seek + separation) * enemy.speed;
        enemy.position += velocity * delta_time;
    }
}

void Destroy(GameState& state) {
    state.Reset();
}

} // namespace

const Scene TEST_SCENE = {.name = "Test", .init = Init, .update = Update, .draw = Draw, .destroy = Destroy};
