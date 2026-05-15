#pragma once

#include "raylib.h"

struct Projectile {
    Vector2 velocity = {};
    Vector2 position = {};
    float life_time = 0;
    float time_alive = 0;
    int damage = 1;
};
