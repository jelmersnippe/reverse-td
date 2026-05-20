#include "spawner_system.hpp"

#include "core/entity_pool.hpp"
#include "entities/enemy.hpp"
#include "game_state.hpp"
#include "raymath.h"

void Update(Slot<Spawner>& spawner_slot, EntityPool<Enemy>& enemies, const float difficulty_scale) {
    Spawner& spawner = spawner_slot.ref;

    const float delta_time = GetFrameTime();
    spawner.time_since_last_spawn += delta_time;

    if (!spawner.initial_spawn_happened) {
        for (int i = 0; i < spawner.initial_spawn; i++) {
            Enemy new_enemy = melee_enemy;
            new_enemy.position = spawner.position;
            new_enemy.home = spawner_slot.handle;
            new_enemy.target_position = spawner.position;
            CreateEntity(enemies, new_enemy);
        }

        spawner.initial_spawn_happened = true;
        spawner.time_since_last_spawn = 0;
        return;
    }

    if (spawner.time_since_last_spawn < (spawner.spawn_cooldown / difficulty_scale)) return;

    const int spawn_count = static_cast<int>((float)spawner.spawn_amount * difficulty_scale);
    for (int i = 0; i < spawn_count; i++) {
        Enemy new_enemy = melee_enemy;
        new_enemy.position = spawner.position;
        new_enemy.home = spawner_slot.handle;
        new_enemy.target_position = spawner.position;
        CreateEntity(enemies, new_enemy);
    }

    spawner.time_since_last_spawn = 0;
}

void UpdateSpawners(GameState& state) {
    for (Slot<Spawner>& spawner : state.spawners.data) {
        if (!spawner.alive) continue;

        Update(spawner, state.enemies, state.difficulty_scale);
    }
}

void DrawSpawners(const EntityPool<Spawner>& spawners) {
    for (const Slot<Spawner>& spawner : spawners.data) {
        if (!spawner.alive) continue;

        const Vector2 spawner_top_left = {.x = spawner.ref.position.x - SPAWNER_SIZE / 2,
                                          .y = spawner.ref.position.y - SPAWNER_SIZE / 2};
        DrawRectangleLines(spawner_top_left.x, spawner_top_left.y, SPAWNER_SIZE, SPAWNER_SIZE, BLACK);
        const int text_width = MeasureText("Spawner", 12);
        DrawText("Spawner", spawner.ref.position.x - text_width / 2, spawner.ref.position.y, 12, BLACK);

        DrawHealth(spawner.ref.position - Vector2{.x = 0, .y = SPAWNER_SIZE / 2 + 10}, spawner.ref.health);
    }
}
