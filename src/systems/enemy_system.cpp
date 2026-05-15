#include "enemy_system.hpp"

#include "game_state.hpp"
#include "globals.hpp"
#include "raymath.h"

void Update(Enemy& enemy, GameState& state) {
    const float delta_time = GetFrameTime();

    enemy.time_since_last_attack += delta_time;
    const float radius = ENEMY_SIZE * ((float)enemy.health.max / (float)BASE_ENEMY_HEALTH);

    // TODO: Change this so it can also target TARGET_TOWER
    // TODO: Fix enemy converging
    if (Vector2Distance(state.player.position, enemy.position) <= (PLAYER_SIZE / 2) + radius + ENEMY_ATTACK_RANGE) {
        // If in range -> stand still and attack
        enemy.velocity = {.x = 0, .y = 0};

        if (enemy.time_since_last_attack >= enemy.attack_cooldown) {
            state.player.health.current -= enemy.damage;
            enemy.time_since_last_attack = 0;
        }
    } else {
        // Else -> move closer
        enemy.velocity = Vector2Normalize(state.player.position - enemy.position) * ENEMY_SPEED;
        enemy.position += enemy.velocity * delta_time;
    }
}

void UpdateEnemies(GameState& state) {
    for (Slot<Enemy>& enemy : state.enemies.data) {
        if (!enemy.alive) continue;

        Update(enemy.ref, state);
    }
}
