#pragma once

#include "raylib.h"
#include "systems/targeting.hpp"

struct Projectile {
    Vector2 velocity = {};
    Vector2 position = {};
    float life_time = 0;
    float time_alive = 0;
    int damage = 0;
    uint32_t flags = TARGET_NONE;
};
