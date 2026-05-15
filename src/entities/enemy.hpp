#pragma once

#include "raylib.h"

#include "core/health.hpp"

struct Enemy {
    Vector2 velocity = {};
    Vector2 position = {};
    Health health = {};
    int damage = 1;
    float attack_cooldown = 2;
    float time_since_last_attack = 0;
};
