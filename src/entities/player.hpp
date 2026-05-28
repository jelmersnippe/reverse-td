#pragma once

#include "raylib.h"

#include "core/health.hpp"

struct Player {
    Vector2 position = {};
    Vector2 direction = {};
    Health health = Health(100);
    int damage = 25;
    float time_since_last_shot = 0;
    float speed = 125;
    float attacking_speed_modifier = 0.5;
};
