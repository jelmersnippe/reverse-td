#pragma once

#include "globals.hpp"
#include "raylib.h"

#include "core/health.hpp"

struct Enemy {
    Vector2 velocity = {};
    Vector2 position = {};
    Health health = Health{.max = BASE_ENEMY_HEALTH, .current = BASE_ENEMY_HEALTH};

    float speed = 100;
    float range = 5;
    float size = 20;

    int damage = 1;
    float attack_cooldown = 2;
    float time_since_last_attack = 0;
};
