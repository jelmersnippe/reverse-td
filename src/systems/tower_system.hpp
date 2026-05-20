#pragma once

#include "core/entity_pool.hpp"
#include "entities/tower.hpp"

struct GameState;

void UpdateTowers(GameState& state);
void DrawTowers(const EntityPool<Tower>& towers, const Camera2D& camera);

int GetScrapValue(const Tower& tower);
