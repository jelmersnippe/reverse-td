#pragma once

#include "core/entity_pool.hpp"
#include "entities/player.hpp"
struct GameState;
struct Camera2D;

void UpdatePlayers(GameState& state);

void DrawPlayers(const EntityPool<Player>& players, const Camera2D& camera);
