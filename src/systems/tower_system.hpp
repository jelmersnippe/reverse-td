#pragma once

#include "core/camera.hpp"
#include "core/entity_pool.hpp"
#include "entities/tower.hpp"

struct GameState;

void UpdateTowers(GameState& state);
void DrawTowers(const EntityPool<Tower>& towers, const MainCamera& camera);

int GetScrapValue(const Tower& tower);
