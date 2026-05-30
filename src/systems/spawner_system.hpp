#pragma once

#include "core/entity_pool.hpp"
#include "entities/spawner.hpp"
struct GameState;

const int SPAWNER_OFFSET = 50;

void UpdateSpawners(GameState& state);

void DrawSpawners(const EntityPool<Spawner>& spawners);
