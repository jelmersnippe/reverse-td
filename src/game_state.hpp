#pragma once

#include "raylib.h"

struct Health {
    int max;
    int current;
};

struct GameState {
    Vector2 player_position = {};
    Health player_health = {};
    Vector2 player_direction = {};
};
