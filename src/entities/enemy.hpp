#pragma once

#include "globals.hpp"
#include "raylib.h"

#include "core/health.hpp"
#include <cstdint>

enum class SeekBehavior : uint32_t {
    SimpleFollow,
    Separation,
    Count
};

enum class AttackBehavior : uint32_t {
    None,
    Melee,
    Count
};

struct Enemy {
    SeekBehavior seek_behavior = SeekBehavior::SimpleFollow;
    AttackBehavior attack_behavior = AttackBehavior::Melee;
    Vector2 position = {};
    Health health = Health{.max = BASE_ENEMY_HEALTH, .current = BASE_ENEMY_HEALTH};

    float speed = 100;
    float range = 5;
    float size = 20;

    int damage = 1;
    float attack_cooldown = 2;
    float time_since_last_attack = 0;
};
