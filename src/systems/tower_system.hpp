#pragma once

#include "entities/tower.hpp"

struct GameState;

void UpdateTowers(GameState& state);

int GetScrapValue(const Tower& tower);
