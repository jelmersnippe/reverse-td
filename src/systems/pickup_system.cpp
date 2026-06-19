#include "pickup_system.hpp"

#include "core/entity_pool.hpp"
#include "core/renderer.hpp"
#include "game_state.hpp"
#include "systems/targeting.hpp"
#include <optional>

void UpdatePickups(GameState& state) {
    const float delta_time = GetFrameTime();

    for (Slot<Pickup>& pickup : state.pickups.data) {
        if (!pickup.alive) continue;

        if (!pickup.ref.target.has_value()) {
            std::optional<Targetable> target =
                find_closest_target(pickup.ref.position, state.targetables, TARGET_PLAYER);

            if (!target.has_value() || target->position.distance_to(pickup.ref.position) > PICKUP_RANGE) continue;

            pickup.ref.target = target;
        }

        Player* active_player = GetEntity(state.players, pickup.ref.target.value().handle);

        if (active_player == nullptr) {
            pickup.ref.target = std::nullopt;
            continue;
        };

        pickup.ref.position +=
            pickup.ref.position.direction_to(active_player->position).normalized() * PICKUP_SPEED * delta_time;

        if (pickup.ref.position.distance_to(active_player->position) < 5) {
            state.currency += pickup.ref.value;
            DestroyEntity(state.pickups, pickup.handle);
        }
    }
}

void DrawPickups(const GameState& state) {
    for (const Slot<Pickup>& pickup : state.pickups.data) {
        if (!pickup.alive) continue;

        render_sprite({"lifeforce", {.x = DEFAULT_SPRITE_SIZE, .y = DEFAULT_SPRITE_SIZE}, {.x = 1, .y = 1}},
                      pickup.ref.position, pickup.ref.angle);
    }
}
