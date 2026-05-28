#pragma once

#include "core/entity_pool.hpp"
#include "entities/pickup.hpp"
struct GameState;

inline float PICKUP_RANGE = 250;
inline float PICKUP_SPEED = 300;

void UpdatePickups(GameState& state);
void DrawPickups(const EntityPool<Pickup>& pickups);
