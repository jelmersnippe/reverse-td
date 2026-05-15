#pragma once

#include "raylib.h"

#include "entities/health.hpp"

struct Player {
    Vector2 position;
    Vector2 direction;
    Health health;
    float time_since_last_shot;
};
