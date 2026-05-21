#pragma once

#include "core/entity_pool.hpp"
#include "raylib.h"

#include "core/health.hpp"
#include <vector>

struct Spawner {
    Vector2 position = {};
    Health health = Health(500);
    float spawn_cooldown = 10;
    int spawn_amount = 1;
    int initial_spawn = 3;
    float time_since_last_spawn = 0;
    bool initial_spawn_happened = false;

    Vector2 rally_position = {};
    int max_spawn = 6;

    std::vector<EntityHandle> active_enemies = {};
};
