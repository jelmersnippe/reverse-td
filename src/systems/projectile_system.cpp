#include "projectile_system.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raymath.h"

// Returns a bool for if it should be destroyed
bool Update(Projectile& projectile, EntityPool<Enemy>& enemies, EntityPool<Spawner>& spawners) {
    const float delta_time = GetFrameTime();

    bool hit = false;

    projectile.time_alive += delta_time;

    if (projectile.time_alive >= projectile.life_time) { return true; }

    const Vector2 old_position = projectile.position;

    projectile.position += projectile.velocity * delta_time;

    for (size_t enemy_index = 0; enemy_index < enemies.data.size(); enemy_index++) {
        Slot<Enemy>& enemy = enemies.data[enemy_index];
        if (!enemy.alive) continue;

        // Projectile trajectory would collide with enemy.
        // This is done with a line instead of point because a projectile could move fast enough
        // to fully pass through an enemy in a frame, thus the point would not be in the circle ever
        // TODO: This only checks center point of projectile. Should also check radius. So cylender/rectangle for
        // path
        const float radius = ENEMY_SIZE * ((float)enemy.ref.health.max / (float)BASE_ENEMY_HEALTH);
        hit = CheckCollisionCircleLine(enemy.ref.position, radius, old_position, projectile.position);

        if (!hit) continue;

        enemy.ref.health.current -= projectile.damage;

        if (enemy.ref.health.current <= 0) {
            DestroyEntity(enemies, {.index = enemy_index, .generation = enemy.generation});

            // TODO: Move elsewhere
            // state.difficulty_scale += 0.02;
            // state.currency += 1;
        }

        return true;
    }

    for (size_t spawner_index = 0; spawner_index < spawners.data.size(); spawner_index++) {
        Slot<Spawner>& spawner = spawners.data[spawner_index];

        hit = CheckCollisionPointRec(
            projectile.position,
            {.x = spawner.ref.position.x, .y = spawner.ref.position.y, .width = SPAWNER_SIZE, .height = SPAWNER_SIZE});

        if (!hit) continue;

        spawner.ref.health.current -= projectile.damage;

        if (spawner.ref.health.current <= 0) {
            DestroyEntity(spawners, {.index = spawner_index, .generation = spawner.generation});

            // TODO: Move elsewhere
            // state.difficulty_scale += 0.1;
            // state.currency += 5;
        }

        return true;
    }

    return false;
}

void UpdateProjectiles(GameState& state) {
    for (size_t projectile_index = 0; projectile_index < state.projectiles.data.size(); projectile_index++) {
        Slot<Projectile>& projectile = state.projectiles.data[projectile_index];

        if (!projectile.alive) continue;

        bool should_destroy = Update(projectile.ref, state.enemies, state.spawners);

        if (should_destroy) {
            DestroyEntity(state.projectiles, {.index = projectile_index, .generation = projectile.generation});
        }
    }
}
