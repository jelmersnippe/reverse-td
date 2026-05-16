#pragma once

#include "globals.hpp"
#include "raylib.h"

#include "core/entity_pool.hpp"
#include "entities/enemy.hpp"
#include "entities/player.hpp"
#include "entities/projectile.hpp"
#include "entities/spawner.hpp"
#include "entities/tower.hpp"
#include <vector>

struct Targetable;

enum class Input {
    LeftMouse,
    RightMouse,
};

const int STARTING_CURRENCY = 1000;
const float STARTING_DIFFICULTY = 1;

struct GameState {
    bool should_exit = false;

    std::vector<Input> inputs = {};
    Player player;
    EntityPool<Projectile> projectiles = {};
    EntityPool<Spawner> spawners = {};
    EntityPool<Enemy> enemies = {};
    EntityPool<Tower> towers = {};

    float difficulty_scale = STARTING_DIFFICULTY;
    Camera2D camera = {.offset = {.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2},
                       .target = {.x = 0, .y = 0},
                       .rotation = 0.0f,
                       .zoom = 1.0f};

    int currency = STARTING_CURRENCY;

    void Reset() {
        player = {};
        inputs.clear();
        spawners.clear();
        enemies.clear();
        towers.clear();
        projectiles.clear();
        difficulty_scale = STARTING_DIFFICULTY;
        currency = STARTING_CURRENCY;

        camera.target = {.x = 0, .y = 0};
    }
};

void apply_damage(GameState& state, Targetable& target, int amount);

struct CollisionResult {
    bool collided = false;
    Vector2 location = {};
};
CollisionResult check_player_collision(GameState& state, Vector2 position);
