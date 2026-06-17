#pragma once

#include "core/data.hpp"
struct Health {
    float max;
    float current;

    float animated_health;

    Health(float start) : animated_health(start) { max = start, current = start; }

    void update() {
        if (animated_health <= current) return;

        animated_health -= (animated_health - current) * 0.2f;
    }
};

void DrawHealth(const Vec2F& position, const Health& health);
