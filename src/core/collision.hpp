#pragma once

#include "core/data.hpp"
#include "core/renderer.hpp"
#include "raylib.h"

enum class ColliderType {
    Circle,
    Box
};

struct Collider {
    ColliderType type;

    // Box
    Rect rect = {};

    // Circle
    Vec2F center = {};
    float radius = 0;

    Collider(Vec2F center, float radius) : type(ColliderType::Circle), center(center), radius(radius) {}
    Collider(Rect rect) : type(ColliderType::Box), rect(rect) {}

    void draw(Vec2F position, Color color = RED) const {
        switch (this->type) {
            case ColliderType::Circle:
                render_circle(position + this->center, radius, color, true);
                break;
            case ColliderType::Box:
                render_rectangle(position + this->rect.position, this->rect.size, color, false, true);
                break;
        }
    }
};

inline bool collision_point_circle(Vec2F point, Vec2F center, float radius) {
    return CheckCollisionPointCircle({.x = point.x, .y = point.y}, {.x = center.x, .y = center.y}, radius);
}

inline bool collision_point_rect(Vec2F point, Rect rect) {
    return CheckCollisionPointRec(
        {.x = point.x, .y = point.y},
        {.x = rect.position.x, .y = rect.position.y, .width = rect.size.x, .height = rect.size.y});
}

inline bool collision_point_collider(Vec2F point, Collider collider) {
    switch (collider.type) {
        case ColliderType::Circle:
            return collision_point_circle(point, collider.center, collider.radius);
            break;
        case ColliderType::Box:
            return collision_point_rect(point, collider.rect);
            break;
    }
}
