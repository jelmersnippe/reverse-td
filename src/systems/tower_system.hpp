#pragma once

#include "entities/tower.hpp"

struct GameState;

void UpdateTowers(GameState& state);
void DrawTowers(const GameState& state);

int GetScrapValue(const Tower& tower);
