#pragma once

#include "core/gen_index.hpp"
#include "entities/enemy.hpp"
#include "raylib.h"

#include "entities/health.hpp"

struct Spawner {
    Vector2 position = {};
    Health health = {};
    float spawn_cooldown = 10;
    int spawn_amount = 1;
    int initial_spawn = 3;
    float time_since_last_spawn = 0;
    bool initial_spawn_happened = false;
};

void Update(Spawner& spawner, EntityPool<Enemy>& enemies, const float difficulty_scale);
