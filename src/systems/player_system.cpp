#include "player_system.hpp"

#include "core/asset_manager.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raylib.h"
#include "raymath.h"
#include <cmath>

void Update(Player& player, GameState& state) {
    const float delta_time = GetFrameTime();

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

        Vector2 hand_offset = {.x = 8, .y = 0};

        // Player
        auto player_sprite = get_sprite("player");
        Rectangle player_source = {.x = 0, .y = 0, .width = 16, .height = 16};
        if (mouse_direction.x < 0) player_source.width = -player_source.width;

        DrawTexturePro(
            player_sprite, player_source,
            {.x = player.ref.position.x, .y = player.ref.position.y, .width = PLAYER_SIZE, .height = PLAYER_SIZE},
            {.x = PLAYER_SIZE / 2, .y = PLAYER_SIZE / 2}, 0, WHITE);

        // Gun
        auto pistol_sprite = get_sprite("pistol");
        Rectangle pistol_source = {
            .x = 0, .y = 0, .width = (float)pistol_sprite.width, .height = (float)pistol_sprite.height};
        Vector2 pistol_origin = {.x = (float)pistol_sprite.width * .5f, .y = (float)pistol_sprite.height * .5f};

        const float rad = mouse_angle * DEG2RAD;
        Vector2 rotated_offset = {.x = cosf(rad) * hand_offset.x - sinf(rad) * hand_offset.y,
                                  .y = sinf(rad) * hand_offset.x + cosf(rad) * hand_offset.y};
        Vector2 weapon_pos = player.ref.position + rotated_offset;
        Rectangle pistol_dest = {.x = weapon_pos.x,
                                 .y = weapon_pos.y,
                                 .width = (float)pistol_sprite.width * 2,
                                 .height = (float)pistol_sprite.height * 2};

        if (mouse_direction.x < 0) {
            pistol_source.height = -pistol_sprite.height;
            pistol_origin.y += pistol_sprite.height;
        }

        DrawTexturePro(pistol_sprite, pistol_source, pistol_dest, pistol_origin, mouse_angle, WHITE);

        // Health
        const Vector2 health_position = player.ref.position - Vector2{.x = 0, .y = PLAYER_SIZE};
        DrawHealth(health_position, player.ref.health);

        if (player.ref.regenerating) {
            const int text_width = MeasureText("Regenerating", 10);
            DrawText("Regenerating", health_position.x - text_width / 2, health_position.y - 10, 10, BLACK);
        }
    }
}
