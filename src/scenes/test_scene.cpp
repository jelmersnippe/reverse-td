#include "scenes/test_scene.hpp"
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

namespace {
void Init(GameState& state) {
    Player player = {.position = Vector2{.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2},
                     .health = {.max = PLAYER_STARTING_HEALTH, .current = PLAYER_STARTING_HEALTH}};
    state.player = player;
    state.camera.target = {player.position};
}

void Draw(const GameState& state) {
    ClearBackground(WHITE);

    BeginMode2D(state.camera);

    DrawRectangle(state.player.position.x - PLAYER_SIZE / 2, state.player.position.y - PLAYER_SIZE / 2, PLAYER_SIZE,
                  PLAYER_SIZE, GREEN);

    for (const Slot<Enemy>& enemy : state.enemies.data) {
        if (!enemy.alive) continue;

        const float radius = ENEMY_SIZE * ((float)enemy.ref.health.max / (float)BASE_ENEMY_HEALTH);
        DrawCircle(enemy.ref.position.x, enemy.ref.position.y, radius, RED);
    }

    EndMode2D();
}

void Update(GameState& state) {
    const float delta_time = GetFrameTime();
    Vector2 velocity = Vector2Normalize(state.player.direction) * PLAYER_SPEED * delta_time;
    state.player.position += velocity;

    state.camera.target = {state.player.position};
}

void Destroy(GameState& state) {
    state.Reset();
}

} // namespace

const Scene TEST_SCENE = {.name = "Test", .init = Init, .update = Update, .draw = Draw, .destroy = Destroy};
