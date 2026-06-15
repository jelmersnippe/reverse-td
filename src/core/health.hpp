#pragma once

#include "core/data.hpp"
struct Health {
    int max;
    int current;

    Health(int start) { max = start, current = start; }
};

void DrawHealth(const Vec2F& position, const Health& health);
