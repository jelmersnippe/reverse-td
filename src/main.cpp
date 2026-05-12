#include "raylib.h"

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
const int TARGET_FPS = 60;

void Update() {};

void Draw() {
    BeginDrawing();
    ClearBackground(RED);
    EndDrawing();
}

void HandleInput() {}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sokoban");
    InitAudioDevice();
    SetExitKey(KEY_NULL);
    SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose()) {
        HandleInput();

        Update();

        Draw();
    }

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
