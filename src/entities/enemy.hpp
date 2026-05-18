#pragma once

#include "globals.hpp"
#include "raylib.h"

#include "core/health.hpp"
#include <cstdint>

enum class EnemyState : uint32_t {
    Wander,
    Seek,
    Attack
};

enum class SeekBehavior : uint32_t {
    SimpleFollow,
    Separation,
    Count
};

enum class AttackBehavior : uint32_t {
    None,
    Melee,
    Ranged,
    Count
};

struct Enemy {
    Color color = RED;
    EnemyState state = EnemyState::Wander;
    SeekBehavior seek_behavior = SeekBehavior::SimpleFollow;
    AttackBehavior attack_behavior = AttackBehavior::Melee;
    Vector2 position = {};
    Health health = Health(BASE_ENEMY_HEALTH);

    float speed = 100;
    float range = 5;
    float size = 20;

    int damage = 1;
    float attack_cooldown = 2;
    float time_since_last_attack = 0;
};

inline const Enemy melee_enemy = {.color = ORANGE,
                                  .seek_behavior = SeekBehavior::Separation,
                                  .attack_behavior = AttackBehavior::Melee,
                                  .health = Health(50),
                                  .speed = 100,
                                  .size = 20,
                                  .damage = 15,
                                  .attack_cooldown = 2};

inline const Enemy fast_enemy = {
    .color = YELLOW,
    .seek_behavior = SeekBehavior::Separation,
    .attack_behavior = AttackBehavior::Melee,
    .health = Health(30),
    .speed = 150,
    .size = 10,
    .damage = 10,
    .attack_cooldown = 2,
};

inline const Enemy ranged_enemy = {
    .color = BLUE,
    .seek_behavior = SeekBehavior::Separation,
    .attack_behavior = AttackBehavior::Ranged,
    .health = Health(40),
    .speed = 75,
    .range = 300,
    .size = 15,
    .damage = 12,
    .attack_cooldown = 1,
};

inline const Enemy tank_enemy = {
    .color = RED,
    .seek_behavior = SeekBehavior::Separation,
    .attack_behavior = AttackBehavior::Melee,
    .health = Health(250),
    .speed = 40,
    .range = 15,
    .size = 60,
    .damage = 30,
    .attack_cooldown = 4,
};
