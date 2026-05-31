#pragma once

#include "raylib.h"

#include "core/health.hpp"

struct Tower {
    Vector2 position = {};
    Health health = Health(100);
    int damage = 20;
    int fire_rate = 120;
    float time_since_last_attack = 0;
    int range = 350;

    bool scrapping = false;
    float time_to_scrap = 3;
    float scrap_time = 0;
};
