#pragma once

#include "raylib.h"

#include "entities/health.hpp"
#include "entities/player.hpp"

struct Enemy {
    Vector2 velocity = {};
    Vector2 position = {};
    Health health = {};
    int damage = 1;
    float attack_cooldown = 2;
    float time_since_last_attack = 0;
};

void Update(Enemy& enemy, Player& player);
