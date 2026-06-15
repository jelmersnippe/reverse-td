#include "player_system.hpp"

#include "core/asset_manager.hpp"
#include "core/renderer.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raylib.h"
#include "raymath.h"
#include <cmath>

void Update(Player& player, GameState& state) {
    const float delta_time = GetFrameTime();

    if (player.direction.x == 0 && player.direction.y == 0) {
        player.animation_player.stop();
    } else {
        player.animation_player.play();
    }
    player.animation_player.update(delta_time);

    player.time_since_last_shot += delta_time;
    player.time_since_last_damage_taken += delta_time;

    const bool can_regen = player.time_since_last_shot >= player.out_of_combat_time &&
                           player.time_since_last_damage_taken >= player.out_of_combat_time &&
                           player.health.current < player.health.max;

    if (!can_regen) {
        player.regenerating = false;
    } else {
        if (!player.regenerating) {
            player.regenerating = true;
            player.time_since_last_regen = 0;
        }

        player.time_since_last_regen += delta_time;

        if (player.time_since_last_regen >= player.regen_time) {
            player.health.current++;

            player.time_since_last_regen -= player.regen_time;
        }
    }

    float speed = player.speed;
    if (player.time_since_last_shot < TIME_BETWEEN_SHOTS) { speed *= player.attacking_speed_modifier; }

    Vector2 velocity = Vector2Normalize(player.direction) * speed * delta_time;

    Vector2 new_position = player.position;

    new_position.x += velocity.x;
    const CollisionResult collision_x = check_player_collision(state, new_position);

    new_position = player.position;
    new_position.y += velocity.y;
    const CollisionResult collision_y = check_player_collision(state, new_position);

    if (!collision_x.collided) player.position.x += velocity.x;
    if (!collision_y.collided) player.position.y += velocity.y;

    player.direction = {.x = 0, .y = 0};

    // TODO: Should show a death state / restart the game
    if (player.health.current <= 0) { state.should_exit = true; }
}

void UpdatePlayers(GameState& state) {
    for (Slot<Player>& player : state.players.data) {
        if (!player.alive) continue;

        Update(player.ref, state);
    }
}

void DrawPlayers(const EntityPool<Player>& players, const Camera2D& camera) {
    for (const Slot<Player>& player : players.data) {
        if (!player.alive) continue;

        const Vector2 mouse_position = GetScreenToWorld2D(GetMousePosition(), camera);
        const Vector2 mouse_direction = mouse_position - player.ref.position;
        float mouse_angle = atan2f(mouse_direction.y, mouse_direction.x) * RAD2DEG;

        // Player
        player.ref.animation_player.draw(Vec2F{player.ref.position.x, player.ref.position.y},
                                         {PLAYER_SIZE, PLAYER_SIZE}, mouse_direction.x < 0);

        // Gun
        // TODO: Extract into rotate_around helper
        Vec2F hand_offset = {.x = 14.0f, .y = 0};
        const float rad = mouse_angle * DEG2RAD;
        Vec2F rotated_offset = {.x = cosf(rad) * hand_offset.x - sinf(rad) * hand_offset.y,
                                .y = sinf(rad) * hand_offset.x + cosf(rad) * hand_offset.y};
        Vec2F weapon_pos = Vec2F{.x = player.ref.position.x, .y = player.ref.position.y} + rotated_offset;

        Vec2 sprite_size = {.x = 16, .y = 16};
        Vec2F render_size = {.x = sprite_size.x * 2.0f, .y = sprite_size.y * 2.0f};
        render_sprite(SpriteInfo("pistol", sprite_size, 0, {.x = false, .y = mouse_direction.x < 0}), weapon_pos,
                      render_size, mouse_angle, WHITE);

        // Health
        const Vector2 health_position = player.ref.position - Vector2{.x = 0, .y = PLAYER_SIZE};
        DrawHealth(health_position, player.ref.health);

        if (player.ref.regenerating) {
            const int text_width = MeasureText("Regenerating", 10);
            DrawText("Regenerating", health_position.x - text_width / 2, health_position.y - 10, 10, BLACK);
        }
    }
}
