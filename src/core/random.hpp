#pragma once

#include "raylib.h"

inline int random_int(int min, int max) {
    return GetRandomValue(min, max);
}

inline float random_float(float min, float max) {
    const float random = (float)GetRandomValue(0, 100) * 0.01f;

    return min + (max - min) * random;
}
