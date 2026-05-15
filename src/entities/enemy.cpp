#include "raylib.h"
#include "raymath.h"

#include "enemy.hpp"
#include "globals.hpp"

void Update(Enemy& enemy, Player& player) {
    const float delta_time = GetFrameTime();

    enemy.time_since_last_attack += delta_time;
    const float radius = ENEMY_SIZE * ((float)enemy.health.max / (float)BASE_ENEMY_HEALTH);

    // TODO: Fix enemy converging
    if (Vector2Distance(player.position, enemy.position) <= (PLAYER_SIZE / 2) + (radius / 2) + ENEMY_ATTACK_RANGE) {
        // If in range -> stand still and attack
        enemy.velocity = {.x = 0, .y = 0};

        if (enemy.time_since_last_attack >= enemy.attack_cooldown) {
            player.health.current -= enemy.damage;
            enemy.time_since_last_attack = 0;
        }
    } else {
        // Else -> move closer
        enemy.velocity = Vector2Normalize(player.position - enemy.position) * ENEMY_SPEED;
        enemy.position += enemy.velocity * delta_time;
    }
}
