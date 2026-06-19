#include "projectile_system.hpp"

#include "core/collision.hpp"
#include "core/entity_pool.hpp"
#include "core/renderer.hpp"
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
            if (!enemy.alive || enemy.ref.dead) continue;

            hit = collision_point_circle(projectile.position, enemy.ref.position, enemy.ref.size);

            if (!hit) continue;

            Targetable target = {.flags = TARGET_ENEMY, .handle = enemy.handle, .position = enemy.ref.position};
            apply_damage(state, target, projectile.damage, projectile.position.direction_to(target.position));

            return true;
        }
    }

    if ((projectile.flags & TARGET_SPAWNER) == TARGET_SPAWNER) {
        for (Slot<Spawner>& spawner : state.spawners.data) {
            if (!spawner.alive) continue;

            hit = collision_point_collider(projectile.position, spawner.ref.collider);

            if (!hit) continue;

            Targetable target = {.flags = TARGET_SPAWNER, .handle = spawner.handle, .position = spawner.ref.position};
            apply_damage(state, target, projectile.damage, projectile.position.direction_to(target.position));

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

            Targetable target = {.flags = TARGET_TOWER, .handle = tower.handle, .position = tower.ref.position};
            apply_damage(state, target, projectile.damage, projectile.position.direction_to(target.position));

            return true;
        }
    }

    if ((projectile.flags & TARGET_PLAYER) == TARGET_PLAYER) {
        for (Slot<Player>& player : state.players.data) {
            if (!player.alive) continue;

            hit = collision_point_collider(projectile.position, player.ref.collider);

            if (!hit) continue;

            Targetable target = {.flags = TARGET_PLAYER, .handle = player.handle, .position = player.ref.position};
            apply_damage(state, target, projectile.damage, projectile.position.direction_to(target.position));

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

void DrawProjectiles(const GameState& state) {
    // TODO: Cull stuff outside of the screen
    for (const Slot<Projectile>& projectile : state.projectiles.data) {
        if (!projectile.alive) continue;

        std::string sprite_name;
        float scale = 2;
        switch (projectile.ref.type) {
            case ProjectileType::Enemy:
                sprite_name = "enemy_projectile";
                break;
            case ProjectileType::Player:
                sprite_name = "player_projectile";
                scale = 4;
                break;
        }

        render_sprite(
            SpriteInfo(sprite_name, {.x = DEFAULT_SPRITE_SIZE, .y = DEFAULT_SPRITE_SIZE}, {.x = scale, .y = scale}),
            projectile.ref.position, projectile.ref.direction.angle());
    }
}
