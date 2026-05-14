#pragma once

#include "raylib.h"
#include <vector>

struct Health {
    int max;
    int current;
};

enum class Input {
    FireWeapon,
};

struct Enemy {
    Vector2 velocity = {};
    Vector2 position = {};
    Health health = {};
    int damage = 1;
    float attack_cooldown = 2;
    float time_since_last_attack = 0;
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
    Vector2 player_position = {};
    Health player_health = {};
    Vector2 player_direction = {};
    float time_since_last_shot = 0;
    std::vector<Input> inputs = {};
    std::vector<Projectile> projectiles = {};
    std::vector<Enemy> enemies = {};
    std::vector<Spawner> spawners = {};

    float difficulty_scale = 1;
};
