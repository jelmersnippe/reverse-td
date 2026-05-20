#include "targeting.hpp"
#include "core/entity_pool.hpp"
#include "game_state.hpp"
#include "raymath.h"
#include <optional>

std::vector<Targetable> build_targetables(const GameState& state) {
    std::vector<Targetable> targetables = {};

    for (uint32_t i = 0; i < state.players.data.size(); i++) {
        const Slot<Player>& player = state.players.data[i];
        if (!player.alive) continue;

        targetables.push_back(Targetable{.flags = TARGET_PLAYER,
                                         .handle = {.index = i, .generation = player.generation},
                                         .position = player.ref.position});
    }

    for (uint32_t i = 0; i < state.enemies.data.size(); i++) {
        const Slot<Enemy>& enemy = state.enemies.data[i];
        if (!enemy.alive) continue;

        targetables.push_back(Targetable{.flags = TARGET_ENEMY,
                                         .handle = {.index = i, .generation = enemy.generation},
                                         .position = enemy.ref.position});
    }

    for (uint32_t i = 0; i < state.spawners.data.size(); i++) {
        const Slot<Spawner>& spawner = state.spawners.data[i];
        if (!spawner.alive) continue;

        targetables.push_back(Targetable{.flags = TARGET_SPAWNER,
                                         .handle = {.index = i, .generation = spawner.generation},
                                         .position = spawner.ref.position});
    }

    for (uint32_t i = 0; i < state.towers.data.size(); i++) {
        const Slot<Tower>& tower = state.towers.data[i];
        if (!tower.alive) continue;

        targetables.push_back(Targetable{.flags = TARGET_TOWER,
                                         .handle = {.index = i, .generation = tower.generation},
                                         .position = tower.ref.position});
    }

    return targetables;
}

std::optional<Targetable> find_closest_target(const Vector2& position, const std::vector<Targetable>& targetables,
                                              const uint32_t target_flags) {
    float closest_distance = std::numeric_limits<float>::max();

    std::optional<Targetable> current_target_handle = std::nullopt;

    for (const Targetable& targetable : targetables) {
        if (!(targetable.flags & target_flags)) continue;

        const float distance = Vector2Distance(position, targetable.position);

        if (distance > closest_distance) continue;

        closest_distance = distance;
        current_target_handle = targetable;
    }

    return current_target_handle;
}
