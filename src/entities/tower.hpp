#pragma once

#include "core/gen_index.hpp"
#include "entities/enemy.hpp"
#include "entities/projectile.hpp"
#include "health.hpp"
#include "raylib.h"

struct Tower {
    Vector2 position = {};
    Health health = {};
    int damage = 1;
    int fire_rate = 200;
    float time_since_last_attack = 0;
    int range = 200;
    EntityHandle target = {};
};

void Update(Tower& tower, EntityPool<Enemy>& enemies, EntityPool<Projectile>& projectiles);
