#include "projectile_system.hpp"

#include "game_state.hpp"
#include "globals.hpp"
#include "raymath.h"
#include "systems/targeting.hpp"

// Returns a bool for if it should be destroyed
bool Update(Projectile& projectile, GameState& state) {
    const float delta_time = GetFrameTime();

    bool hit = false;

    projectile.time_alive += delta_time;

    if (projectile.time_alive >= projectile.life_time) { return true; }

    const Vector2 old_position = projectile.position;

    projectile.position += projectile.velocity * delta_time;

    for (size_t enemy_index = 0; enemy_index < state.enemies.data.size(); enemy_index++) {
        Slot<Enemy>& enemy = state.enemies.data[enemy_index];
        if (!enemy.alive) continue;

        // Projectile trajectory would collide with enemy.
        // This is done with a line instead of point because a projectile could move fast enough
        // to fully pass through an enemy in a frame, thus the point would not be in the circle ever
        // TODO: This only checks center point of projectile. Should also check radius. So cylender/rectangle for
        // path
        const float radius = enemy.ref.size * ((float)enemy.ref.health.max / (float)BASE_ENEMY_HEALTH);
        hit = CheckCollisionCircleLine(enemy.ref.position, radius, old_position, projectile.position);

        if (!hit) continue;

        Targetable target = {
            .flags = TARGET_ENEMY, .handle = {.index = enemy_index, .generation = enemy.generation}, .position = {}};
        apply_damage(state, target, projectile.damage);

        return true;
    }

    for (size_t spawner_index = 0; spawner_index < state.spawners.data.size(); spawner_index++) {
        Slot<Spawner>& spawner = state.spawners.data[spawner_index];

        const Vector2 spawner_top_left =
            Vector2{.x = spawner.ref.position.x - SPAWNER_SIZE / 2, .y = spawner.ref.position.y - SPAWNER_SIZE / 2};
        hit = CheckCollisionPointRec(
            projectile.position,
            {.x = spawner_top_left.x, .y = spawner_top_left.y, .width = SPAWNER_SIZE, .height = SPAWNER_SIZE});

        if (!hit) continue;

        Targetable target = {.flags = TARGET_SPAWNER,
                             .handle = {.index = spawner_index, .generation = spawner.generation},
                             .position = {}};
        apply_damage(state, target, projectile.damage);

        return true;
    }

    return false;
}

void UpdateProjectiles(GameState& state) {
    for (size_t projectile_index = 0; projectile_index < state.projectiles.data.size(); projectile_index++) {
        Slot<Projectile>& projectile = state.projectiles.data[projectile_index];

        if (!projectile.alive) continue;

        bool should_destroy = Update(projectile.ref, state);

        if (should_destroy) {
            DestroyEntity(state.projectiles, {.index = projectile_index, .generation = projectile.generation});
        }
    }
}
