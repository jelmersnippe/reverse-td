#include "projectile_system.hpp"

#include "core/asset_manager.hpp"
#include "core/collision.hpp"
#include "core/entity_pool.hpp"
#include "entities/projectile.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "systems/targeting.hpp"

// Returns a bool for if it should be destroyed
bool Update(Projectile& projectile, GameState& state) {
    const float delta_time = GetFrameTime();

    bool hit = false;

    projectile.time_alive += delta_time;

    if (projectile.time_alive >= projectile.life_time) { return true; }

    projectile.position += projectile.direction * projectile.speed * delta_time;

    if ((projectile.flags & TARGET_ENEMY) == TARGET_ENEMY) {
        for (Slot<Enemy>& enemy : state.enemies.data) {
            if (!enemy.alive) continue;

            hit = collision_point_circle(projectile.position, enemy.ref.position, enemy.ref.size);

            if (!hit) continue;

            Targetable target = {.flags = TARGET_ENEMY, .handle = enemy.handle, .position = {}};
            apply_damage(state, target, projectile.damage);

            return true;
        }
    }

    if ((projectile.flags & TARGET_SPAWNER) == TARGET_SPAWNER) {
        for (Slot<Spawner>& spawner : state.spawners.data) {
            if (!spawner.alive) continue;

            const Vec2F spawner_top_left = {.x = spawner.ref.position.x - SPAWNER_SIZE / 2,
                                            .y = spawner.ref.position.y - SPAWNER_SIZE / 2};
            hit = collision_point_rect(projectile.position,
                                       {.position = spawner_top_left, .size = {.x = SPAWNER_SIZE, .y = SPAWNER_SIZE}});

            if (!hit) continue;

            Targetable target = {.flags = TARGET_SPAWNER, .handle = spawner.handle, .position = {}};
            apply_damage(state, target, projectile.damage);

            return true;
        }
    }

    if ((projectile.flags & TARGET_TOWER) == TARGET_TOWER) {
        for (Slot<Tower>& tower : state.towers.data) {
            if (!tower.alive) continue;

            const Vec2F tower_top_left = {.x = tower.ref.position.x - TOWER_SIZE / 2,
                                          .y = tower.ref.position.y - TOWER_SIZE / 2};
            hit = collision_point_rect(projectile.position,
                                       {.position = tower_top_left, .size = {.x = TOWER_SIZE, .y = TOWER_SIZE}});

            if (!hit) continue;

            Targetable target = {.flags = TARGET_TOWER, .handle = tower.handle, .position = {}};
            apply_damage(state, target, projectile.damage);

            return true;
        }
    }

    if ((projectile.flags & TARGET_PLAYER) == TARGET_PLAYER) {
        for (Slot<Player>& player : state.players.data) {
            if (!player.alive) continue;

            const Vec2F player_top_left = {.x = player.ref.position.x - TOWER_SIZE / 2,
                                           .y = player.ref.position.y - TOWER_SIZE / 2};
            hit = collision_point_rect(projectile.position,
                                       {.position = player_top_left, .size = {.x = TOWER_SIZE, .y = TOWER_SIZE}});

            if (!hit) continue;

            Targetable target = {.flags = TARGET_PLAYER, .handle = player.handle, .position = {}};
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

        Texture2D sprite;
        float scale = 2;
        switch (projectile.ref.type) {
            case ProjectileType::Enemy:
                sprite = get_sprite("enemy_projectile");
                break;
            case ProjectileType::Player:
                sprite = get_sprite("player_projectile");
                scale = 4;
                break;
        }

        Rectangle source = {.x = 0, .y = 0, .width = (float)sprite.width, .height = (float)sprite.height};
        Rectangle dest = {.x = projectile.ref.position.x,
                          .y = projectile.ref.position.y,
                          .width = sprite.width * scale,
                          .height = sprite.height * scale};
        Vector2 origin = {.x = dest.width * 0.5f, .y = dest.height * 0.5f};

        DrawTexturePro(sprite, source, dest, origin,
                       atan2f(projectile.ref.direction.y, projectile.ref.direction.x) * RAD2DEG, WHITE);
    }
}
