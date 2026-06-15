#pragma once

#include "core/animation_player.hpp"
#include "core/health.hpp"

struct Player {
    AnimationPlayer animation_player =
        AnimationPlayer({.sprite_name = "player", .sprite_size = {.x = 16, .y = 16}, .frame_count = 4});
    Vec2F position = {};
    Vec2F direction = {};
    Health health = Health(100);
    int damage = 25;
    float time_since_last_shot = 0;
    float speed = 200;
    float attacking_speed_modifier = 0.5;

    float time_since_last_damage_taken = 0;

    float out_of_combat_time = 10;

    float time_since_last_regen = 0;
    float regen_time = 0.4f;
    bool regenerating = false;
};
