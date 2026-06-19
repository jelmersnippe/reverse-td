#pragma once

struct GameState;

const int SPAWNER_OFFSET = 50;

void UpdateSpawners(GameState& state);

void DrawSpawners(const GameState& state);
void DrawSpawnersDebug(const GameState& state);
