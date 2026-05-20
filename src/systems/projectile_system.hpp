#pragma once

#include "entities/projectile.hpp"
struct GameState;

void UpdateProjectiles(GameState& state);

void DrawProjectiles(const EntityPool<Projectile>& projectiles);
