#pragma once

#include "core/entity_pool.hpp"
#include "core/health.hpp"
#include <vector>

enum class SpawnerState {
    Idle,
    Rallying,
    UnderAttack,
};

const int RALLY_DISTANCE = 250;
const float RESET_TIME = 10;

struct Spawner {
    Vec2F position = {};
    Health health = Health(500);
    float spawn_cooldown = 10;
    int spawn_amount = 1;
    int initial_spawn = 3;
    float time_since_last_spawn = 0;
    bool initial_spawn_happened = false;

    Vec2F rally_position = {};
    int max_spawn = 6;

    std::vector<EntityHandle> active_enemies = {};

    SpawnerState state = SpawnerState::Idle;

    float time_since_last_damage_taken = 0;
    float under_attack_spawn_modifier = 0.5;
};
