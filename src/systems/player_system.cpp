#include "player_system.hpp"

#include "game_state.hpp"
#include "raylib.h"
#include "raymath.h"

void Update(Player& player, GameState& state) {
    const float delta_time = GetFrameTime();

    player.time_since_last_shot += delta_time;

    Vector2 velocity = Vector2Normalize(player.direction) * PLAYER_SPEED * delta_time;

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

void DrawPlayers(const EntityPool<Player>& players) {
    for (const Slot<Player>& player : players.data) {
        if (!player.alive) continue;

        DrawRectangle(player.ref.position.x - PLAYER_SIZE / 2, player.ref.position.y - PLAYER_SIZE / 2, PLAYER_SIZE,
                      PLAYER_SIZE, GREEN);
        DrawHealth(player.ref.position - Vector2{.x = 0, .y = PLAYER_SIZE}, player.ref.health);
    }
}
