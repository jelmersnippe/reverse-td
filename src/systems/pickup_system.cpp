#include "pickup_system.hpp"

#include "core/entity_pool.hpp"
#include "game_state.hpp"
#include "raylib.h"
#include "raymath.h"
#include "systems/targeting.hpp"
#include <optional>

void UpdatePickups(GameState& state) {
    const float delta_time = GetFrameTime();

    const auto targetables = build_targetables(state);

    for (Slot<Pickup>& pickup : state.pickups.data) {
        if (!pickup.alive) continue;

        if (!pickup.ref.target.has_value()) {
            std::optional<Targetable> target = find_closest_target(pickup.ref.position, targetables, TARGET_PLAYER);

            if (!target.has_value() || Vector2Distance(target->position, pickup.ref.position) > PICKUP_RANGE) continue;

            pickup.ref.target = target;
        }

        Player* active_player = GetEntity(state.players, pickup.ref.target.value().handle);

        if (active_player == nullptr) {
            pickup.ref.target = std::nullopt;
            continue;
        };

        pickup.ref.position +=
            Vector2Normalize(active_player->position - pickup.ref.position) * PICKUP_SPEED * delta_time;

        if (Vector2Distance(pickup.ref.position, active_player->position) < 5) {
            state.currency += pickup.ref.value;
            DestroyEntity(state.pickups, pickup.handle);
        }
    }
}

void DrawPickups(const EntityPool<Pickup>& pickups) {
    for (const Slot<Pickup>& pickup : pickups.data) {
        if (!pickup.alive) continue;

        DrawRectangle(pickup.ref.position.x - 5, pickup.ref.position.y - 5, 10, 10, PINK);
    }
}
