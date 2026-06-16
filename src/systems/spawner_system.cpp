#include "spawner_system.hpp"

#include "core/entity_pool.hpp"
#include "core/random.hpp"
#include "core/renderer.hpp"
#include "entities/enemy.hpp"
#include "entities/spawner.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "systems/threat_director.hpp"
#include <algorithm>
#include <cassert>

Enemy get_spawn_option(std::vector<SpawnOption>& spawn_table) {

    const float rng = random_float(0, 1);
    float weight = 0;

    for (const SpawnOption& option : spawn_table) {
        weight += option.weight;

        if (rng < weight) return option.enemy;
    }

    assert(spawn_table.size() > 0 && "spawn_table was empty");
    assert(weight == 1 && "spawn_table weights did not add up to 1");

    // Default enemy, should never happen
    return melee_enemy;
};

void spawn_enemies(Slot<Spawner>& spawner_slot, EntityPool<Enemy>& enemies, std::vector<SpawnOption>& spawn_table,
                   const int count) {
    Spawner& spawner = spawner_slot.ref;

    int active_count = 0;
    for (const EntityHandle& handle : spawner.active_enemies) {
        if (IsValidEntity(enemies, handle)) active_count++;
    }

    int spawn_count = count;
    int max_spawn = spawner.max_spawn;
    if (spawner.state == SpawnerState::UnderAttack) {
        spawn_count += 1;
        max_spawn = max_spawn * 0.5;
    }

    const int limited_count = std::min(max_spawn - active_count, spawn_count);

    for (int i = 0; i < limited_count; i++) {
        Enemy new_enemy = get_spawn_option(spawn_table);

        const float random_x = random_float(-SPAWNER_OFFSET, SPAWNER_OFFSET);
        const float random_y = random_float(-SPAWNER_OFFSET, SPAWNER_OFFSET);

        new_enemy.position = spawner.position + Vec2F{.x = random_x, .y = random_y};
        new_enemy.home = spawner_slot.handle;
        new_enemy.target_position = spawner.position;
        const EntityHandle created_enemy = CreateEntity(enemies, new_enemy);

        spawner.active_enemies.push_back(created_enemy);
    }

    spawner.time_since_last_spawn = 0;
}

void Update(Slot<Spawner>& spawner_slot, EntityPool<Enemy>& enemies, std::vector<SpawnOption>& spawn_table) {
    Spawner& spawner = spawner_slot.ref;

    const float delta_time = GetFrameTime();
    spawner.time_since_last_spawn += delta_time;
    spawner.time_since_last_damage_taken += delta_time;

    if (!spawner.initial_spawn_happened) {
        spawn_enemies(spawner_slot, enemies, spawn_table, spawner.initial_spawn);
        spawner.initial_spawn_happened = true;
    }

    float spawn_cooldown = spawner.spawn_cooldown;
    if (spawner.state == SpawnerState::UnderAttack) spawn_cooldown *= spawner.under_attack_spawn_modifier;

    if (spawner.time_since_last_spawn >= spawn_cooldown) {
        spawn_enemies(spawner_slot, enemies, spawn_table, spawner.spawn_amount);
    }

    std::vector<Enemy*> resolved_enemies;
    std::vector<size_t> active_entities_to_remove = {};

    for (size_t i = 0; i < spawner.active_enemies.size(); i++) {
        const EntityHandle handle = spawner.active_enemies[i];
        Enemy* enemy = GetEntity(enemies, handle);

        if (enemy == nullptr || !enemy->home.IsValid()) {
            active_entities_to_remove.push_back(i);
            continue;
        }

        resolved_enemies.push_back(enemy);
    }

    for (const size_t index : active_entities_to_remove) {
        spawner.active_enemies.erase(spawner.active_enemies.begin() + index);
    }

    if (spawner.state == SpawnerState::Rallying) {
        // Max enemies and all rallied
        const bool max_enemies_rallied =
            resolved_enemies.size() == spawner.max_spawn &&
            std::ranges::all_of(resolved_enemies, [](Enemy* enemy) { return enemy->rallied; });

        for (Enemy* enemy : resolved_enemies) {
            if (max_enemies_rallied) {
                enemy->state = EnemyState::Seek;
                enemy->home = EntityHandle{};
            } else {
                enemy->state = EnemyState::Rally;
            }
        }

        // Allow spawner to spawn enemies again
        if (max_enemies_rallied) {
            spawner.active_enemies.clear();
            spawner.state = SpawnerState::Idle;
        }
    } else if (spawner.state == SpawnerState::UnderAttack) {
        if (spawner.time_since_last_damage_taken >= RESET_TIME) {
            spawner.state = SpawnerState::Idle;
            return;
        }

        for (Enemy* enemy : resolved_enemies) {
            enemy->state = EnemyState::Seek;
            enemy->home = EntityHandle{};
        }
    }
}

void UpdateSpawners(GameState& state) {
    for (Slot<Spawner>& spawner : state.spawners.data) {
        if (!spawner.alive) continue;

        Update(spawner, state.enemies, state.threat_director.spawn_table);
    }
}

void DrawSpawners(const EntityPool<Spawner>& spawners) {
    for (const Slot<Spawner>& spawner : spawners.data) {
        if (!spawner.alive) continue;

        render_rectangle(spawner.ref.position, {.x = SPAWNER_SIZE, .y = SPAWNER_SIZE}, BLACK, true);
        const int text_width = MeasureText("Spawner", 12);
        DrawText("Spawner", spawner.ref.position.x - text_width / 2, spawner.ref.position.y, 12, BLACK);

        std::string state_text;
        switch (spawner.ref.state) {
            case SpawnerState::Rallying:
                state_text = "Rallying";
                break;
            case SpawnerState::UnderAttack:
                state_text = "Under Attack";
                break;
            default:
                state_text = "Idle";
                break;
        }
        DrawText(state_text.c_str(), spawner.ref.position.x - text_width / 2, spawner.ref.position.y + 20, 12, BLACK);

        if (spawner.ref.state == SpawnerState::Rallying) {
            DrawCircle(spawner.ref.rally_position.x, spawner.ref.rally_position.y, 5, BLACK);
            // DrawLineDashed(spawner.ref.position, spawner.ref.rally_position, 10, 10, BLACK);
        }

        DrawHealth(spawner.ref.position - Vec2F{.x = 0, .y = SPAWNER_SIZE / 2 + 10}, spawner.ref.health);
    }
}
