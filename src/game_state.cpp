#include "game_state.hpp"
#include "core/entity_pool.hpp"
#include "entities/enemy.hpp"
#include "globals.hpp"
#include "raylib.h"
#include "systems/targeting.hpp"

void kill_entity(GameState& state, Targetable& target) {
    switch (target.flags) {
        case TARGET_ENEMY: {
            DestroyEntity(state.enemies, target.handle);
            state.currency += 1;
            state.threat_director.threat += 0.05f;
            break;
        }
        case TARGET_SPAWNER: {
            DestroyEntity(state.spawners, target.handle);
            state.currency += 5;
            state.threat_director.threat += 3.0f;
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
            enemy->state = EnemyState::Seek;
            break;
        }
        case TARGET_SPAWNER: {
            Spawner* spawner = GetEntity(state.spawners, target.handle);
            if (spawner != nullptr) health = &spawner->health;
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
        const Rectangle tower_rect = {.x = tower.ref.position.x - TOWER_SIZE / 2,
                                      .y = tower.ref.position.y - TOWER_SIZE / 2,
                                      .width = TOWER_SIZE,
                                      .height = TOWER_SIZE};

        if (CheckCollisionRecs(tower_rect, player_rect))
            return CollisionResult{.collided = true, .location = tower.ref.position};
    }

    for (const Slot<Spawner>& spawner : state.spawners.data) {
        const Rectangle spawner_rect = {.x = spawner.ref.position.x - SPAWNER_SIZE / 2,
                                        .y = spawner.ref.position.y - SPAWNER_SIZE / 2,
                                        .width = SPAWNER_SIZE,
                                        .height = SPAWNER_SIZE};

        if (CheckCollisionRecs(spawner_rect, player_rect))
            return CollisionResult{.collided = true, .location = spawner.ref.position};
    }

    return CollisionResult{};
}
