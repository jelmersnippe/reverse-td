#pragma once

#include "raylib.h"
#include <optional>
#include <vector>

#include "core/entity_pool.hpp"

struct GameState;

enum TargetFlags : uint32_t {
    TARGET_NONE = 1 << 0,
    TARGET_PLAYER = 1 << 1,
    TARGET_ENEMY = 1 << 2,
    TARGET_SPAWNER = 1 << 3,
    TARGET_TOWER = 1 << 4
};

struct Targetable {
    uint32_t flags;
    EntityHandle handle;
    Vector2 position;
};

std::vector<Targetable> build_targetables(const GameState& state);

std::optional<Targetable> find_closest_target(const Vector2& position, const std::vector<Targetable>& targetables,
                                              const uint32_t target_flags);
