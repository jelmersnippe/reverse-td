#pragma once

#include "core/data.hpp"
#include "raylib.h"

using MainCamera = Camera2D;

inline Vec2F get_mouse_world_position(MainCamera camera) {
    const Vector2 mouse_position = GetScreenToWorld2D(GetMousePosition(), camera);

    return Vec2F::from_raylib(mouse_position);
}

inline Vec2F get_world_position(Vec2F position, MainCamera camera) {
    const Vector2 world_position = GetScreenToWorld2D(position.to_raylib(), camera);

    return Vec2F::from_raylib(world_position);
}
