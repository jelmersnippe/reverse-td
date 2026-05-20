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

    if (Vector2Distance(tower.position, target.position) > tower.range) return;

    // Attacking time
    if (tower.time_since_last_attack >= 60.0 / tower.fire_rate) {
        const Vector2 direction = target.position - tower.position;
        CreateEntity(state.projectiles, {.velocity = Vector2Normalize(direction) * PROJECTILE_SPEED,
                                         .position = tower.position,
                                         .life_time = 2.0,
                                         .damage = tower.damage,
                                         .flags = TARGET_ENEMY | TARGET_SPAWNER});
        tower.time_since_last_attack = 0;
    }
}

void UpdateTowers(GameState& state) {
    for (Slot<Tower>& tower : state.towers.data) {
        if (!tower.alive) continue;

        Update(tower.ref, state);
    }
}

int GetScrapValue(const Tower& tower) {
    const float default_scrap_value = (TOWER_COST * 0.9);
    const float modifier = (float)tower.health.current / (float)tower.health.max;
    return default_scrap_value * modifier;
}
