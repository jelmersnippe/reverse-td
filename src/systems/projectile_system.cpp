#include "projectile_system.hpp"

#include "core/entity_pool.hpp"
#include "entities/projectile.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raylib.h"
#include "raymath.h"
#include "systems/targeting.hpp"

// Returns a bool for if it should be destroyed
bool Update(Projectile& projectile, GameState& state) {
    const float delta_time = GetFrameTime();

    bool hit = false;

    projectile.time_alive += delta_time;

    if (projectile.time_alive >= projectile.life_time) { return true; }

    projectile.position += projectile.velocity * delta_time;

    if ((projectile.flags & TARGET_ENEMY) == TARGET_ENEMY) {
        for (size_t enemy_index = 0; enemy_index < state.enemies.data.size(); enemy_index++) {
            Slot<Enemy>& enemy = state.enemies.data[enemy_index];
            if (!enemy.alive) continue;

            hit = CheckCollisionPointCircle(projectile.position, enemy.ref.position, enemy.ref.size);

            if (!hit) continue;

            Targetable target = {.flags = TARGET_ENEMY,
                                 .handle = {.index = enemy_index, .generation = enemy.generation},
                                 .position = {}};
            apply_damage(state, target, projectile.damage);

            return true;
        }
    }

    if ((projectile.flags & TARGET_SPAWNER) == TARGET_SPAWNER) {
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
    }

    if ((projectile.flags & TARGET_TOWER) == TARGET_TOWER) {
        for (size_t tower_index = 0; tower_index < state.towers.data.size(); tower_index++) {
            Slot<Tower>& tower = state.towers.data[tower_index];

            const Vector2 tower_top_left =
                Vector2{.x = tower.ref.position.x - TOWER_SIZE / 2, .y = tower.ref.position.y - TOWER_SIZE / 2};
            hit = CheckCollisionPointRec(
                projectile.position,
                {.x = tower_top_left.x, .y = tower_top_left.y, .width = TOWER_SIZE, .height = TOWER_SIZE});

            if (!hit) continue;

            Targetable target = {.flags = TARGET_TOWER,
                                 .handle = {.index = tower_index, .generation = tower.generation},
                                 .position = {}};
            apply_damage(state, target, projectile.damage);

            return true;
        }
    }

    if ((projectile.flags & TARGET_PLAYER) == TARGET_PLAYER) {
        for (size_t player_index = 0; player_index < state.players.data.size(); player_index++) {
            Slot<Player>& player = state.players.data[player_index];

            const Vector2 player_top_left =
                Vector2{.x = player.ref.position.x - TOWER_SIZE / 2, .y = player.ref.position.y - TOWER_SIZE / 2};
            hit = CheckCollisionPointRec(
                projectile.position,
                {.x = player_top_left.x, .y = player_top_left.y, .width = TOWER_SIZE, .height = TOWER_SIZE});

            if (!hit) continue;

            Targetable target = {.flags = TARGET_PLAYER,
                                 .handle = {.index = player_index, .generation = player.generation},
                                 .position = {}};
            apply_damage(state, target, projectile.damage);

            return true;
        }
    }

    return false;
}

void UpdateProjectiles(GameState& state) {
    for (Slot<Projectile>& projectile : state.projectiles.data) {
        if (!projectile.alive) continue;

        bool should_destroy = Update(projectile.ref, state);

        if (should_destroy) { DestroyEntity(state.projectiles, projectile.handle); }
    }
}

void DrawProjectiles(const EntityPool<Projectile>& projectiles) {
    // TODO: Cull stuff outside of the screen
    for (const Slot<Projectile>& projectile : projectiles.data) {
        if (!projectile.alive) continue;

        DrawCircle(projectile.ref.position.x, projectile.ref.position.y, PROJECTILE_SIZE, ORANGE);
    }
}
