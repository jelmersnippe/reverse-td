#pragma once

#include "core/entity_pool.hpp"
#include "entities/enemy.hpp"
struct GameState;

inline float PERSONAL_SPACE = 50;

void UpdateEnemies(GameState& state);
void DrawEnemies(const EntityPool<Enemy>& enemies);
