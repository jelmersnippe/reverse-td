#include "player_system.hpp"

#include "core/input.hpp"
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
        float mouse_angle =
            atan2f(mouse_position.y - player.ref.position.y, mouse_position.x - player.ref.position.x) * RAD2DEG;
        DrawRectanglePro(
            Rectangle{.x = player.ref.position.x, .y = player.ref.position.y - 5.0f, .width = 50, .height = 10},
            {.x = 0, .y = 5}, mouse_angle, BLACK);
        DrawRectangle(player.ref.position.x - PLAYER_SIZE / 2, player.ref.position.y - PLAYER_SIZE / 2, PLAYER_SIZE,
                      PLAYER_SIZE, GREEN);
        const Vector2 health_position = player.ref.position - Vector2{.x = 0, .y = PLAYER_SIZE};
        DrawHealth(health_position, player.ref.health);

        if (player.ref.regenerating) {

            const int text_width = MeasureText("Regenerating", 10);
            DrawText("Regenerating", health_position.x - text_width / 2, health_position.y - 10, 10, BLACK);
        }
    }
}
