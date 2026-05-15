#include "raylib.h"
#include "raymath.h"

#include <vector>

#include "core/gen_index.hpp"
#include "globals.hpp"
#include "tower.hpp"

void Update(Tower& tower, EntityPool<Enemy>& enemies, EntityPool<Projectile>& projectiles) {
    const float delta_time = GetFrameTime();

    tower.time_since_last_attack += delta_time;

    Enemy* target = GetEntity(enemies, tower.target);

    // Check if target left range
    if (target != nullptr) {
        const float distance = Vector2Distance(tower.position, target->position);

        if (distance > tower.range) target = nullptr;
    }

    // Acquire target
    if (target == nullptr) {
        for (uint32_t i = 0; i < enemies.data.size(); i++) {
            Slot<Enemy>& enemy = enemies.data[i];
            if (!enemy.alive) continue;

            const float distance = Vector2Distance(tower.position, enemy.ref.position);
            if (distance <= tower.range) {
                tower.target = EntityHandle{.index = i, .generation = enemy.generation};
                target = &enemy.ref;
                break;
            }
        }
    }

    // Target acquiring failed
    if (target == nullptr) return;

    // Attacking time
    if (tower.time_since_last_attack >= 60.0 / tower.fire_rate) {
        const Vector2 tower_center = tower.position + Vector2{.x = TOWER_SIZE / 2, .y = TOWER_SIZE / 2};
        const Vector2 direction = target->position - tower_center;
        CreateEntity(
            projectiles,
            {.velocity = Vector2Normalize(direction) * PROJECTILE_SPEED, .position = tower_center, .life_time = 2.0});
        tower.time_since_last_attack = 0;
    }
}
