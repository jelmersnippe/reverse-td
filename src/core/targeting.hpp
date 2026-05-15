#pragma once

#include "raylib.h"
#include <vector>

#include "core/gen_index.hpp"

struct GameState;

enum TargetFlags : uint32_t {
    TARGET_PLAYER = 1 << 0,
    TARGET_ENEMY = 1 << 1,
    TARGET_SPAWNER = 1 << 2,
    TARGET_TOWER = 1 << 3
};

struct Targetable {
    uint32_t flags;
    EntityHandle handle;
    Vector2 position;
};

std::vector<Targetable> build_targetables(const GameState& state);

Targetable find_closest_target(const Vector2& position, const std::vector<Targetable>& targetables,
                               const uint32_t target_flags);
