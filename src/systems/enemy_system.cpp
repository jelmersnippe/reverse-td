#include "enemy_system.hpp"

#include "core/entity_pool.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raymath.h"
#include "systems/targeting.hpp"

void Update(Enemy& enemy, GameState& state) {
    const float delta_time = GetFrameTime();

    enemy.time_since_last_attack += delta_time;
    const float radius = enemy.range * ((float)enemy.health.max / (float)BASE_ENEMY_HEALTH);

    Targetable target = find_closest_target(enemy.position, build_targetables(state), TARGET_TOWER | TARGET_PLAYER);

    Vector2 target_position = enemy.position;

    switch (target.flags) {
        case TARGET_PLAYER: {
            target_position = state.player.position;
            break;
        }
        case TARGET_TOWER: {
            Tower* tower = GetEntity(state.towers, target.handle);
            if (tower != nullptr) target_position = tower->position;
            break;
        }
        default:
            break;
    }

    // TODO: Fix enemy converging
    if (Vector2Distance(target_position, enemy.position) <= radius * 2 + enemy.range) {
        // If in range -> stand still and attack
        enemy.velocity = {.x = 0, .y = 0};

        if (enemy.time_since_last_attack >= enemy.attack_cooldown) {
            apply_damage(state, target, enemy.damage);
            enemy.time_since_last_attack = 0;
        }
    } else {
        // Else -> move closer
        enemy.velocity = Vector2Normalize(target_position - enemy.position) * enemy.speed;
        enemy.position += enemy.velocity * delta_time;
    }
}

void UpdateEnemies(GameState& state) {
    for (Slot<Enemy>& enemy : state.enemies.data) {
        if (!enemy.alive) continue;

        Update(enemy.ref, state);
    }
}
