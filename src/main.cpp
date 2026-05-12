#include "raylib.h"
#include "raymath.h"

#include "game_state.hpp"

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
const int TARGET_FPS = 60;

const int PLAYER_STARTING_HEALTH = 10;
const int PLAYER_SIZE = 50;
const int PLAYER_SPEED = 200;

// Player with movement and shooting
// Following camera (?)
// Simple enemies
// Dropping turrets
// Increasing difficulty

void Update(GameState& state) {
    const float delta_time = GetFrameTime();

    const Vector2 normalized_direction = Vector2Normalize(state.player_direction);

    state.player_position.x += normalized_direction.x * PLAYER_SPEED * delta_time;
    state.player_position.y += normalized_direction.y * PLAYER_SPEED * delta_time;
};

void Draw(const GameState& state) {
    BeginDrawing();
    ClearBackground(WHITE);

    DrawRectangle(state.player_position.x, state.player_position.y, PLAYER_SIZE, PLAYER_SIZE, GREEN);

    EndDrawing();
}

void HandleInput(GameState& state) {
    Vector2 player_direction = {.x = 0, .y = 0};

    if (IsKeyDown(KEY_A)) { player_direction.x -= 1; }
    if (IsKeyDown(KEY_D)) { player_direction.x += 1; }
    if (IsKeyDown(KEY_W)) { player_direction.y -= 1; }
    if (IsKeyDown(KEY_S)) { player_direction.y += 1; }

    state.player_direction = player_direction;
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sokoban");
    InitAudioDevice();
    SetTargetFPS(TARGET_FPS);

    GameState state = {.player_position = Vector2{.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2},
                       .player_health = {.max = PLAYER_STARTING_HEALTH, .current = PLAYER_STARTING_HEALTH}};

    while (!WindowShouldClose()) {
        HandleInput(state);

        Update(state);

        Draw(state);
    }
    CloseAudioDevice();

    CloseWindow();

    return 0;
}
