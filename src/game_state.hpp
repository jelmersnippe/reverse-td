#pragma once

#include "entities/pickup.hpp"
#include "globals.hpp"
#include "raylib.h"

#include "core/entity_pool.hpp"
#include "entities/enemy.hpp"
#include "entities/player.hpp"
#include "entities/projectile.hpp"
#include "entities/spawner.hpp"
#include "entities/tower.hpp"
#include "systems/threat_director.hpp"
#include <vector>

struct Targetable;

enum class Input {
    W,
    A,
    S,
    D,
    LeftMouse,
    RightMouse,
    X,
    One,
    Two,
    Three,
    Four,
    MouseScrollUp,
    MouseScrollDown,
};

const int STARTING_CURRENCY = 0;

struct GameState {
    bool should_exit = false;

    EntityHandle active_player = {};

    std::vector<Targetable> targetables;

    std::vector<Input> inputs = {};
    EntityPool<Player> players;
    EntityPool<Projectile> projectiles = {};
    EntityPool<Spawner> spawners = {};
    EntityPool<Enemy> enemies = {};
    EntityPool<Tower> towers = {};
    EntityPool<Pickup> pickups = {};
    ThreatDirector threat_director = {};

    int currency = STARTING_CURRENCY;

    Camera2D camera = {.offset = {.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2},
                       .target = {.x = 0, .y = 0},
                       .rotation = 0.0f,
                       .zoom = 1.0f};

    void Reset() {
        active_player = {};
        inputs.clear();
        players.clear();
        projectiles.clear();
        spawners.clear();
        enemies.clear();
        towers.clear();
        threat_director = {};

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
