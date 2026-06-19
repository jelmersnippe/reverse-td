#pragma once

struct GameState;
struct Camera2D;

void UpdatePlayers(GameState& state);

void DrawPlayers(const GameState& state);
void DrawPlayersDebug(const GameState& state);
