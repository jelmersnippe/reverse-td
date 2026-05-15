#pragma once

#include "raylib.h"

#include "core/gen_index.hpp"
#include "entities/enemy.hpp"
#include "entities/spawner.hpp"

struct Projectile {
    Vector2 velocity = {};
    Vector2 position = {};
    float life_time = 0;
    float time_alive = 0;
    int damage = 1;
};

bool Update(Projectile& projectile, EntityPool<Enemy>& enemies, EntityPool<Spawner>& spawners);
