#include "threat_director.hpp"
#include "core/entity_pool.hpp"
#include "entities/enemy.hpp"
#include "game_state.hpp"
#include "raylib.h"
#include "raymath.h"

const int MIN_SPAWNER_RANGE = 300;
const int MAX_SPAWNER_RANGE = 500;

Spawner* get_random_spawner(EntityPool<Spawner>& spawners) {
    if (spawners.data.size() <= 0) return nullptr;

    while (true) {
        const int spawner_index = GetRandomValue(0, spawners.data.size() - 1);

        Slot<Spawner>& slot = spawners.data[spawner_index];

        if (slot.alive) return &slot.ref;
    }
}

void UpdateThreatDirector(GameState& state) {
    ThreatDirector& director = state.threat_director;
    const float delta_time = GetFrameTime();

    director.time_to_next_spawner_spread -= delta_time;

    director.threat += 0.3f * delta_time;

    director.spawn_table.clear();

    if (director.threat < 10) {
        director.spawn_table.push_back({.enemy = melee_enemy, .weight = 1});
    } else if (director.threat < 20) {
        director.spawn_table.push_back({.enemy = melee_enemy, .weight = 0.7});
        director.spawn_table.push_back({.enemy = fast_enemy, .weight = 0.3});
    } else if (director.threat < 35) {
        director.spawn_table.push_back({.enemy = melee_enemy, .weight = 0.5});
        director.spawn_table.push_back({.enemy = fast_enemy, .weight = 0.3});
        director.spawn_table.push_back({.enemy = ranged_enemy, .weight = 0.2});
    } else {
        director.spawn_table.push_back({.enemy = melee_enemy, .weight = 0.3});
        director.spawn_table.push_back({.enemy = fast_enemy, .weight = 0.3});
        director.spawn_table.push_back({.enemy = ranged_enemy, .weight = 0.3});
        director.spawn_table.push_back({.enemy = tank_enemy, .weight = 0.1});
    }

    if (director.time_to_next_spawner_spread <= 0) {
        Spawner* parent_spawner = get_random_spawner(state.spawners);

        if (parent_spawner == nullptr) return;

        const float random_x = GetRandomValue(MIN_SPAWNER_RANGE, MAX_SPAWNER_RANGE);
        const int x_negative = GetRandomValue(0, 1);
        const float random_y = GetRandomValue(MIN_SPAWNER_RANGE, MAX_SPAWNER_RANGE);
        const int y_negative = GetRandomValue(0, 1);

        Vector2 offset = {.x = random_x, .y = random_y};
        if (x_negative == 1) offset.x = -random_x;
        if (y_negative == 1) offset.y = -random_y;

        CreateEntity(state.spawners, Spawner{.position = parent_spawner->position + offset, .initial_spawn = 0});

        director.time_to_next_spawner_spread = GetRandomValue(20, 40);
    }
}
