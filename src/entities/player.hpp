#pragma once

#include "core/animation_player.hpp"
#include "core/collision.hpp"
#include "core/health.hpp"
#include "core/transform.hpp"
#include "entities/weapon.hpp"

const float PLAYER_SCALE = 3;

struct Player {
    Transform2D transform;
    Collider collider =
        Collider(Rect{.position = Vec2F{.x = -3, .y = -3}, .size = Vec2F{.x = 5, .y = 8}} * PLAYER_SCALE);

    Health health = Health(100);
    Weapon weapon = {.sprite = {"pistol", {.x = DEFAULT_SPRITE_SIZE, .y = DEFAULT_SPRITE_SIZE}, {.x = 2, .y = 2}},
                     .sound_name = "pistol",
                     .projectile_spawn_point = Vec2F{.x = 16, .y = -2.0f}};
    AnimationPlayer animation_player = AnimationPlayer({
        .sprite = SpriteInfo("player", {.x = DEFAULT_SPRITE_SIZE, .y = DEFAULT_SPRITE_SIZE}, {.x = 8, .y = 9},
                             {.x = PLAYER_SCALE, .y = PLAYER_SCALE}),
        .frame_count = 4,
    });

    Vec2F direction = {};
    float speed = 200;
    float attacking_speed_modifier = 0.5;

    float time_since_last_damage_taken = 0;

    float out_of_combat_time = 10;

    float time_since_last_regen = 0;
    float regen_time = 0.4f;
    bool regenerating = false;
};
