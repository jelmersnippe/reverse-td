#include "entities/enemy.hpp"
#include "raylib.h"
#include "raymath.h"

#include "core/gen_index.hpp"
#include "globals.hpp"
#include "spawner.hpp"

void Update(Spawner& spawner, EntityPool<Enemy>& enemies, const float difficulty_scale) {
    const float delta_time = GetFrameTime();
    spawner.time_since_last_spawn += delta_time;

    if (!spawner.initial_spawn_happened) {
        for (int i = 0; i < spawner.initial_spawn; i++) {
            CreateEntity(enemies, {.position = spawner.position + Vector2{.x = static_cast<float>(0.5 * i), .y = 0},
                                   .health = {.max = static_cast<int>(BASE_ENEMY_HEALTH * difficulty_scale),
                                              .current = static_cast<int>(BASE_ENEMY_HEALTH * difficulty_scale)},
                                   .damage = static_cast<int>(1 * difficulty_scale)});
        }

        spawner.initial_spawn_happened = true;
        spawner.time_since_last_spawn = 0;
        return;
    }

    if (spawner.time_since_last_spawn < (spawner.spawn_cooldown / difficulty_scale)) return;

    const int spawn_count = static_cast<int>((float)spawner.spawn_amount * difficulty_scale);
    for (int i = 0; i < spawn_count; i++) {
        CreateEntity(enemies, {.position = spawner.position + Vector2{.x = static_cast<float>(0.5 * i), .y = 0},
                               .health = {.max = static_cast<int>(BASE_ENEMY_HEALTH * difficulty_scale),
                                          .current = static_cast<int>(BASE_ENEMY_HEALTH * difficulty_scale)},
                               .damage = static_cast<int>(1 * difficulty_scale)});
    }

    spawner.time_since_last_spawn = 0;
}
