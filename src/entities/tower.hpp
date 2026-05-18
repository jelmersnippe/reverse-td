#pragma once

#include "raylib.h"

#include "core/health.hpp"

struct Tower {
    Vector2 position = {};
    Health health = Health(10);
    int damage = 1;
    int fire_rate = 60;
    float time_since_last_attack = 0;
    int range = 200;
};
