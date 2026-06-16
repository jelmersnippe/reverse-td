#pragma once

#include "core/entity_pool.hpp"
#include "globals.hpp"

#include "core/health.hpp"
#include "systems/targeting.hpp"
#include <cstdint>
#include <optional>

enum class EnemyState : uint32_t {
    Wander,
    Rally,
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
    EntityHandle home = {};
    SeekBehavior seek_behavior = SeekBehavior::SimpleFollow;
    AttackBehavior attack_behavior = AttackBehavior::Melee;
    Vec2F position = {};
    // Used for wander+rally
    Vec2F target_position = {};
    bool rallied = false;
    // Used for seek + attack
    std::optional<Targetable> target = std::nullopt;
    Health health = Health(BASE_ENEMY_HEALTH);

    float remaining_idle_time = 0;

    float speed = 100;
    float range = 5;
    float size = 30;
    float aggro_range = 300;

    int damage = 1;
    float attack_cooldown = 2;
    float time_since_last_attack = 0;

    int value = 1;
};

inline const Enemy melee_enemy = {.color = ORANGE,
                                  .seek_behavior = SeekBehavior::Separation,
                                  .attack_behavior = AttackBehavior::Melee,
                                  .health = Health(50),
                                  .speed = 175,
                                  .size = 25,
                                  .damage = 15,
                                  .attack_cooldown = 2};

inline const Enemy fast_enemy = {
    .color = YELLOW,
    .seek_behavior = SeekBehavior::Separation,
    .attack_behavior = AttackBehavior::Melee,
    .health = Health(30),
    .speed = 250,
    .size = 20,
    .damage = 10,
    .attack_cooldown = 2,
};

inline const Enemy ranged_enemy = {
    .color = BLUE,
    .seek_behavior = SeekBehavior::Separation,
    .attack_behavior = AttackBehavior::Ranged,
    .health = Health(40),
    .speed = 150,
    .range = 300,
    .size = 20,
    .damage = 12,
    .attack_cooldown = 1,
};

inline const Enemy tank_enemy = {
    .color = RED,
    .seek_behavior = SeekBehavior::Separation,
    .attack_behavior = AttackBehavior::Melee,
    .health = Health(250),
    .speed = 100,
    .range = 15,
    .size = 50,
    .damage = 30,
    .attack_cooldown = 4,
};
