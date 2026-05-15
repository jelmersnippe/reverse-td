#pragma once

#include "raylib.h"

#include "core/health.hpp"

struct Spawner {
    Vector2 position = {};
    Health health = {};
    float spawn_cooldown = 10;
    int spawn_amount = 1;
    int initial_spawn = 3;
    float time_since_last_spawn = 0;
    bool initial_spawn_happened = false;
};
