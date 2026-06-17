#pragma once

#include "core/data.hpp"
#include "core/health.hpp"

struct Tower {
    Vec2F position = {};
    Vec2F target_position = {};
    Health health = Health(100);
    int damage = 20;
    float fire_rate = 120;
    float time_since_last_attack = 0;
    float range = 400;

    bool scrapping = false;
    float time_to_scrap = 3;
    float scrap_time = 0;
};
