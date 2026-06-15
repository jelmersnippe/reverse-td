#pragma once

#include "core/data.hpp"
#include "raylib.h"

using MainCamera = Camera2D;

inline Vec2F get_mouse_world_position(MainCamera camera) {
    const Vector2 mouse_position = GetScreenToWorld2D(GetMousePosition(), camera);

    return Vec2F{.x = mouse_position.x, .y = mouse_position.y};
}

inline Vec2F get_world_position(Vec2F position, MainCamera camera) {
    const Vector2 world_position = GetScreenToWorld2D({.x = 0, .y = 0}, camera);

    return Vec2F{.x = world_position.x, .y = world_position.y};
}
