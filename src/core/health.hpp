#pragma once

#include "raylib.h"
#include <string>
struct Health {
    int max;
    int current;

    Health(int start) { max = start, current = start; }
};

void DrawHealth(const Vector2& position, const Health& health);
