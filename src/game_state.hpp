#pragma once

#include "raylib.h"

#include "core/entity_pool.hpp"
#include "entities/enemy.hpp"
#include "entities/player.hpp"
#include "entities/projectile.hpp"
#include "entities/spawner.hpp"
#include "entities/tower.hpp"
#include <vector>

enum class Input {
    FireWeapon,
    DropTower,
};

struct GameState {
    bool should_exit = false;

    std::vector<Input> inputs = {};
    Player player;
    EntityPool<Projectile> projectiles = {};
    EntityPool<Spawner> spawners = {};
    EntityPool<Enemy> enemies = {};
    EntityPool<Tower> towers = {};

    float difficulty_scale = 1;
    Camera2D camera = {};

    int currency = 0;
};
