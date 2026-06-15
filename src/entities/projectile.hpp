#pragma once

#include "systems/targeting.hpp"

enum class ProjectileType {
    Player,
    Enemy
};

struct Projectile {
    ProjectileType type = ProjectileType::Player;
    Vec2F direction = {};
    Vec2F position = {};
    float life_time = 0;
    float time_alive = 0;
    float speed = 1500;
    int damage = 0;
    uint32_t flags = TARGET_NONE;
};
