#pragma once

#include "core/camera.hpp"
#include "core/entity_pool.hpp"
#include "entities/enemy.hpp"
#include "entities/pickup.hpp"
#include "entities/player.hpp"
#include "entities/projectile.hpp"
#include "entities/spawner.hpp"
#include "entities/tower.hpp"
#include "systems/threat_director.hpp"
#include <vector>

struct Targetable;

const int STARTING_CURRENCY = 10;

struct GameState {
    bool debug_enabled = false;
    bool should_exit = false;

    EntityHandle active_player = {};

    std::vector<Targetable> targetables;

    EntityPool<Player> players;
    EntityPool<Projectile> projectiles = {};
    EntityPool<Spawner> spawners = {};
    EntityPool<Enemy> enemies = {};
    EntityPool<Tower> towers = {};
    EntityPool<Pickup> pickups = {};
    ThreatDirector threat_director = {};

    int currency = STARTING_CURRENCY;

    MainCamera camera = {.offset = {.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2},
                         .target = {.x = 0, .y = 0},
                         .rotation = 0.0f,
                         .zoom = 1.0f};

    void Reset() {
        active_player = {};
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

void apply_damage(GameState& state, Targetable& target, int amount, Vec2F direction);

struct CollisionResult {
    bool collided = false;
    Vec2F location = {};
};
CollisionResult check_player_collision(GameState& state, Vec2F position);
