#pragma once

#include "core/particles.hpp"
#include "core/renderer.hpp"
#include "globals.hpp"

struct Weapon {
    SpriteInfo sprite;
    std::string sound_name;
    Vec2F projectile_spawn_point;
    float angle = 0;
    Vec2F position = {};
    Vec2F offset = {.x = 14.0f, .y = 6.0f};
    Vec2F current_knockback{};
    float spread = 10;
    Vec2F recoil = {.x = -6, .y = 0};
    float recovery_time = 0.2f;
    int damage = 25;
    float time_between_shots = TIME_BETWEEN_SHOTS;

    float time_since_last_shot = TIME_BETWEEN_SHOTS;

    ParticleSystem particles = {};
};
