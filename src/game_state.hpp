#pragma once

#include "raylib.h"
#include <cstdint>
#include <limits>
#include <vector>

struct Health {
    int max;
    int current;
};

enum class Input {
    FireWeapon,
    DropTower,
};

const uint32_t INVALID_INDEX = std::numeric_limits<uint32_t>::max();
struct EntityHandle {
    size_t index = INVALID_INDEX;
    uint32_t generation = 0;

    bool IsValid() const { return index != INVALID_INDEX; }
};

template <typename T> struct Slot {
    T ref;
    uint32_t generation;
    bool alive;
};

template <typename T> struct EntityPool {
    std::vector<Slot<T>> data;
    std::vector<uint32_t> free_indices;
};

struct Enemy {
    Vector2 velocity = {};
    Vector2 position = {};
    Health health = {};
    int damage = 1;
    float attack_cooldown = 2;
    float time_since_last_attack = 0;
};

struct Tower {
    Vector2 position = {};
    Health health = {};
    int damage = 1;
    int fire_rate = 200;
    float time_since_last_attack = 0;
    int range = 200;
    EntityHandle target = {};
};

struct Spawner {
    Vector2 position = {};
    Health health = {};
    float spawn_cooldown = 10;
    int spawn_amount = 1;
    int initial_spawn = 3;
    float time_since_last_spawn = 0;
    bool initial_spawn_happened = false;
};

struct Projectile {
    Vector2 velocity = {};
    Vector2 position = {};
    float life_time = 0;
    float time_alive = 0;
    int damage = 1;
};

struct GameState {
    bool should_exit = false;

    Vector2 player_position = {};
    Health player_health = {};
    Vector2 player_direction = {};
    float time_since_last_shot = 0;
    std::vector<Input> inputs = {};
    std::vector<Projectile> projectiles = {};
    std::vector<Spawner> spawners = {};
    EntityPool<Enemy> enemies = {};
    EntityPool<Tower> towers = {};

    float difficulty_scale = 1;
    Camera2D camera = {};

    int currency = 0;
};

template <typename T> EntityHandle CreateEntity(EntityPool<T>& pool, T entity) {
    uint32_t index = -1;
    uint32_t generation = -1;
    if (!pool.free_indices.empty()) {
        index = pool.free_indices.back();
        pool.free_indices.pop_back();

        Slot<T>& slot = pool.data[index];
        slot.ref = entity;
        slot.alive = true;
        generation = slot.generation;
    } else {
        index = pool.data.size();
        generation = 0;
        pool.data.push_back(Slot<T>{.ref = entity, .generation = 0, .alive = true});
    }

    return {.index = index, .generation = generation};
}

template <typename T> void DestroyEntity(EntityPool<T>& pool, EntityHandle handle) {
    if (!IsValidEntity(pool, handle)) return;

    pool.data[handle.index].generation += 1;
    pool.data[handle.index].alive = false;
    pool.free_indices.push_back(handle.index);
}

template <typename T> T* GetEntity(EntityPool<T>& pool, EntityHandle handle) {
    if (!handle.IsValid() || !IsValidEntity(pool, handle)) return nullptr;

    return &pool.data[handle.index].ref;
}

template <typename T> bool IsValidEntity(const EntityPool<T>& pool, EntityHandle handle) {
    return handle.index < pool.data.size() && pool.data[handle.index].generation == handle.generation;
}
