#include "game_state.hpp"
#include "core/entity_pool.hpp"
#include "entities/enemy.hpp"
#include "entities/spawner.hpp"
#include "globals.hpp"
#include "raylib.h"
#include "systems/targeting.hpp"

void kill_entity(GameState& state, Targetable& target) {
    switch (target.flags) {
        case TARGET_ENEMY: {
            Enemy* killed_enemy = GetEntity(state.enemies, target.handle);

            if (killed_enemy == nullptr) break;

            const int value = killed_enemy->value;
            const Vector2 position = killed_enemy->position;

            DestroyEntity(state.enemies, target.handle);
            state.threat_director.threat += 0.005f;

            CreateEntity(state.pickups, Pickup{.position = position, .value = value});
            break;
        }
        case TARGET_SPAWNER: {
            Spawner* killed_spawner = GetEntity(state.spawners, target.handle);

            if (killed_spawner == nullptr) break;

            const int value = 5;
            const Vector2 position = killed_spawner->position;

            DestroyEntity(state.spawners, target.handle);
            for (int i = 0; i < value; i++) {
                int random_x = GetRandomValue(0, 50) - 25;
                int random_y = GetRandomValue(0, 50) - 25;
                CreateEntity(
                    state.pickups,
                    Pickup{.position = Vector2{.x = position.x + random_x, .y = position.y + random_y}, .value = 1});
            }
            state.threat_director.threat += 0.03f;
            break;
        }
        case TARGET_PLAYER: {
            DestroyEntity(state.players, target.handle);
            break;
        }
        case TARGET_TOWER: {
            DestroyEntity(state.towers, target.handle);
            break;
        }
        case TARGET_NONE:
            return;
    }
}

void apply_damage(GameState& state, Targetable& target, int amount) {
    Health* health = nullptr;
    switch (target.flags) {
        case TARGET_ENEMY: {
            Enemy* enemy = GetEntity(state.enemies, target.handle);
            if (enemy == nullptr) break;

            health = &enemy->health;

            state.threat_director.threat_active = true;

            Spawner* home = GetEntity(state.spawners, enemy->home);
            if (home != nullptr) {
                home->state = SpawnerState::UnderAttack;
                home->time_since_last_damage_taken = 0;
            }

            enemy->state = EnemyState::Seek;
            enemy->home = EntityHandle{};
            break;
        }
        case TARGET_SPAWNER: {
            Spawner* spawner = GetEntity(state.spawners, target.handle);
            if (spawner == nullptr) break;

            health = &spawner->health;

            state.threat_director.threat_active = true;

            spawner->state = SpawnerState::UnderAttack;
            spawner->time_since_last_damage_taken = 0;
            break;
        }
        case TARGET_PLAYER: {
            Player* player = GetEntity(state.players, target.handle);
            if (player != nullptr) health = &player->health;
            break;
        }
        case TARGET_TOWER: {
            Tower* tower = GetEntity(state.towers, target.handle);
            if (tower != nullptr) health = &tower->health;
            break;
        }
        case TARGET_NONE:
            return;
    }

    if (health == nullptr) return;

    health->current -= amount;

    if (health->current > 0) return;

    kill_entity(state, target);
}

// Checks against all towers and spawners
CollisionResult check_player_collision(GameState& state, Vector2 position) {
    const Rectangle player_rect = {.x = position.x - PLAYER_SIZE / 2,
                                   .y = position.y - PLAYER_SIZE / 2,
                                   .width = PLAYER_SIZE,
                                   .height = PLAYER_SIZE};

    for (const Slot<Tower>& tower : state.towers.data) {
        if (!tower.alive) continue;
        const Rectangle tower_rect = {.x = tower.ref.position.x - TOWER_SIZE / 2,
                                      .y = tower.ref.position.y - TOWER_SIZE / 2,
                                      .width = TOWER_SIZE,
                                      .height = TOWER_SIZE};

        if (CheckCollisionRecs(tower_rect, player_rect))
            return CollisionResult{.collided = true, .location = tower.ref.position};
    }

    for (const Slot<Spawner>& spawner : state.spawners.data) {
        if (!spawner.alive) continue;
        const Rectangle spawner_rect = {.x = spawner.ref.position.x - SPAWNER_SIZE / 2,
                                        .y = spawner.ref.position.y - SPAWNER_SIZE / 2,
                                        .width = SPAWNER_SIZE,
                                        .height = SPAWNER_SIZE};

        if (CheckCollisionRecs(spawner_rect, player_rect))
            return CollisionResult{.collided = true, .location = spawner.ref.position};
    }

    return CollisionResult{};
}
