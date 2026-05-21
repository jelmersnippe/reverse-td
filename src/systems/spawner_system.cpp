#include "spawner_system.hpp"

#include "core/entity_pool.hpp"
#include "entities/enemy.hpp"
#include "game_state.hpp"
#include "raylib.h"
#include "raymath.h"
#include "systems/threat_director.hpp"
#include <algorithm>
#include <cassert>

Enemy get_spawn_option(std::vector<SpawnOption>& spawn_table) {
    const float rng = (float)GetRandomValue(0, 100) / 100.0f;
    float weight = 0;

    for (const SpawnOption& option : spawn_table) {
        weight += option.weight;

        if (rng < weight) return option.enemy;
    }

    assert(spawn_table.size() > 0 && "spawn_table was empty");
    assert(weight == 1 && "spawn_table weights did not add up to 1");

    // Default enemy, should never happen
    return melee_enemy;
};

void spawn_enemies(Slot<Spawner>& spawner_slot, EntityPool<Enemy>& enemies, std::vector<SpawnOption>& spawn_table,
                   const int count) {
    Spawner& spawner = spawner_slot.ref;

    int active_count = 0;
    for (const EntityHandle& handle : spawner.active_enemies) {
        if (IsValidEntity(enemies, handle)) active_count++;
    }

    const int limited_count = std::min(spawner.max_spawn - active_count, count);

    for (int i = 0; i < limited_count; i++) {
        Enemy new_enemy = get_spawn_option(spawn_table);
        new_enemy.position = spawner.position;
        new_enemy.home = spawner_slot.handle;
        new_enemy.target_position = spawner.position;
        const EntityHandle created_enemy = CreateEntity(enemies, new_enemy);

        spawner.active_enemies.push_back(created_enemy);
    }

    spawner.time_since_last_spawn = 0;
}

void Update(Slot<Spawner>& spawner_slot, EntityPool<Enemy>& enemies, std::vector<SpawnOption>& spawn_table) {
    Spawner& spawner = spawner_slot.ref;

    const float delta_time = GetFrameTime();
    spawner.time_since_last_spawn += delta_time;

    if (!spawner.initial_spawn_happened) {
        spawn_enemies(spawner_slot, enemies, spawn_table, spawner.initial_spawn);
        spawner.initial_spawn_happened = true;
        return;
    }

    if (spawner.time_since_last_spawn < spawner.spawn_cooldown) return;

    spawn_enemies(spawner_slot, enemies, spawn_table, spawner.spawn_amount);
}

void UpdateSpawners(GameState& state) {
    for (Slot<Spawner>& spawner : state.spawners.data) {
        if (!spawner.alive) continue;

        Update(spawner, state.enemies, state.threat_director.spawn_table);
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
