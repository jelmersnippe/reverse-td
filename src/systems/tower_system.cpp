#include "tower_system.hpp"
#include "core/targeting.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raymath.h"

void Update(Tower& tower, GameState& state) {
    const float delta_time = GetFrameTime();

    tower.time_since_last_attack += delta_time;

    // TODO: Change this so it can also shoot at TARGET_SPAWNER
    Enemy* target = GetEntity(state.enemies, tower.target.handle);

    // Check if target left range
    if (target != nullptr) {
        const float distance = Vector2Distance(tower.position, target->position);

        if (distance > tower.range) target = nullptr;
    }

    // Acquire target
    if (target == nullptr) {
        Targetable targetable = find_closest_target(tower.position, build_targetables(state), TARGET_ENEMY);
        tower.target = targetable;
        target = GetEntity(state.enemies, targetable.handle);
    }

    // Target acquiring failed
    if (target == nullptr) return;

    // Attacking time
    if (tower.time_since_last_attack >= 60.0 / tower.fire_rate) {
        const Vector2 tower_center = tower.position + Vector2{.x = TOWER_SIZE / 2, .y = TOWER_SIZE / 2};
        const Vector2 direction = target->position - tower_center;
        CreateEntity(
            state.projectiles,
            {.velocity = Vector2Normalize(direction) * PROJECTILE_SPEED, .position = tower_center, .life_time = 2.0});
        tower.time_since_last_attack = 0;
    }
}

void UpdateTowers(GameState& state) {
    for (Slot<Tower>& tower : state.towers.data) {
        if (!tower.alive) continue;

        Update(tower.ref, state);
    }
}
