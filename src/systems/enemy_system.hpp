#pragma once

#include "core/entity_pool.hpp"
#include "entities/enemy.hpp"
struct GameState;

inline float PERSONAL_SPACE = 50;
inline float WANDER_RANGE = 250;
inline float MIN_IDLE_TIME = 1.0;
inline float MAX_IDLE_TIME = 5.0;
inline float MIN_RALLY_IDLE_TIME = 0.5;
inline float MAX_RALLY_IDLE_TIME = 2.0;
inline float WANDER_SPEED_MODIFIER = 0.5;
inline float WANDER_AROUND_RALLY = 100.0;

void UpdateEnemies(GameState& state);
void DrawEnemies(const EntityPool<Enemy>& enemies);
