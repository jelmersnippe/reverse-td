#pragma once

#include "core/animation_player.hpp"
#include "core/health.hpp"
#include "entities/weapon.hpp"

struct Player {
    AnimationPlayer animation_player =
        AnimationPlayer({.sprite_name = "player", .sprite_size = {.x = 16, .y = 16}, .frame_count = 4});
    Vec2F position = {};
    Vec2F direction = {};
    Health health = Health(100);
    Weapon weapon = {.sprite = {"pistol", {.x = 16, .y = 16}},
                     .sound_name = "pistol",
                     .projectile_spawn_point = Vec2F{.x = 16, .y = -2.0f}};
    float speed = 200;
    float attacking_speed_modifier = 0.5;

    float time_since_last_damage_taken = 0;

    float out_of_combat_time = 10;

    float time_since_last_regen = 0;
    float regen_time = 0.4f;
    bool regenerating = false;
};
