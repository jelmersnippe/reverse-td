#include "spawner_system.hpp"

#include "entities/enemy.hpp"
#include "game_state.hpp"

void Update(Spawner& spawner, EntityPool<Enemy>& enemies, const float difficulty_scale) {
    const float delta_time = GetFrameTime();
    spawner.time_since_last_spawn += delta_time;

    if (!spawner.initial_spawn_happened) {
        for (int i = 0; i < spawner.initial_spawn; i++) {
            Enemy new_enemy = melee_enemy;
            new_enemy.position = spawner.position;
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
        CreateEntity(enemies, new_enemy);
    }

    spawner.time_since_last_spawn = 0;
}

void UpdateSpawners(GameState& state) {
    for (Slot<Spawner>& spawner : state.spawners.data) {
        if (!spawner.alive) continue;

        Update(spawner.ref, state.enemies, state.difficulty_scale);
    }
}
