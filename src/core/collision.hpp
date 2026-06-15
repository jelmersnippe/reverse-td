#pragma once

#include "core/data.hpp"
#include "raylib.h"

inline bool collision_point_circle(Vec2F point, Vec2F center, float radius) {
    return CheckCollisionPointCircle({.x = point.x, .y = point.y}, {.x = center.x, .y = center.y}, radius);
}

inline bool collision_point_rect(Vec2F point, Rect rect) {
    return CheckCollisionPointRec(
        {.x = point.x, .y = point.y},
        {.x = rect.position.x, .y = rect.position.y, .width = rect.size.x, .height = rect.size.y});
}
