#pragma once

#include "raylib.h"
#include "systems/targeting.hpp"

enum class ProjectileType {
    Player,
    Enemy
};

struct Projectile {
    ProjectileType type = ProjectileType::Player;
    Vector2 direction = {};
    Vector2 position = {};
    float life_time = 0;
    float time_alive = 0;
    float speed = 1500;
    int damage = 0;
    uint32_t flags = TARGET_NONE;
};
