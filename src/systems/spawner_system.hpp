#pragma once

#include "core/entity_pool.hpp"
#include "entities/spawner.hpp"
struct GameState;

void UpdateSpawners(GameState& state);

void DrawSpawners(const EntityPool<Spawner>& spawners);
