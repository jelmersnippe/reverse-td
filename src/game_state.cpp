#include "game_state.hpp"
#include "core/entity_pool.hpp"
#include "systems/targeting.hpp"

void kill_entity(GameState& state, Targetable& target) {
    switch (target.flags) {
        case TARGET_ENEMY: {
            DestroyEntity(state.enemies, target.handle);
            state.currency += 1;
            state.difficulty_scale += 0.01;
            break;
        }
        case TARGET_SPAWNER: {
            DestroyEntity(state.spawners, target.handle);
            state.currency += 5;
            state.difficulty_scale += 0.05;
            break;
        }
        case TARGET_PLAYER: {
            state.should_exit = true;
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
            if (enemy != nullptr) health = &enemy->health;
            break;
        }
        case TARGET_SPAWNER: {
            Spawner* spawner = GetEntity(state.spawners, target.handle);
            if (spawner != nullptr) health = &spawner->health;
            break;
        }
        case TARGET_PLAYER: {
            health = &state.player.health;
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
