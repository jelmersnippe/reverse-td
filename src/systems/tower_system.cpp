#include "tower_system.hpp"

#include "game_state.hpp"
#include "globals.hpp"
#include "raymath.h"
#include "systems/targeting.hpp"

void Update(Tower& tower, GameState& state) {
    const float delta_time = GetFrameTime();

    tower.time_since_last_attack += delta_time;

    // TODO: Store in Tower and only update every x frames / when out of range
    const Targetable target =
        find_closest_target(tower.position, build_targetables(state), TARGET_ENEMY | TARGET_SPAWNER);

    Vector2 target_position = tower.position;
    switch (target.flags) {
        case TARGET_ENEMY: {
            Enemy* enemy = GetEntity(state.enemies, target.handle);
            if (enemy != nullptr) target_position = enemy->position;
            break;
        }
        case TARGET_SPAWNER: {
            Spawner* spawner = GetEntity(state.spawners, target.handle);
            if (spawner != nullptr) target_position = spawner->position;
            break;
        }
        default:
            break;
    }

    if (target.position == tower.position) return;

    // Attacking time
    if (tower.time_since_last_attack >= 60.0 / tower.fire_rate) {
        const Vector2 direction = target_position - tower.position;
        CreateEntity(
            state.projectiles,
            {.velocity = Vector2Normalize(direction) * PROJECTILE_SPEED, .position = tower.position, .life_time = 2.0});
        tower.time_since_last_attack = 0;
    }
}

void UpdateTowers(GameState& state) {
    for (Slot<Tower>& tower : state.towers.data) {
        if (!tower.alive) continue;

        Update(tower.ref, state);
    }
}
