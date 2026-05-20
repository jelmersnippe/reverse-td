#pragma once

#include "core/entity_pool.hpp"
#include "entities/player.hpp"
struct GameState;

void UpdatePlayers(GameState& state);

void DrawPlayers(const EntityPool<Player>& players);
