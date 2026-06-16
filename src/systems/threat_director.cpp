#include "threat_director.hpp"

#include "core/entity_pool.hpp"
#include "core/random.hpp"
#include "entities/enemy.hpp"
#include "entities/spawner.hpp"
#include "game_state.hpp"

const int MIN_SPAWNER_RANGE = 300;
const int MAX_SPAWNER_RANGE = 500;

Spawner* get_random_spawner(EntityPool<Spawner>& spawners) {
    if (spawners.data.size() <= 0) return nullptr;

    while (true) {
        const int spawner_index = random_int(0, (int)spawners.data.size() - 1);

        Slot<Spawner>& slot = spawners.data[spawner_index];

        if (slot.alive) return &slot.ref;
    }
}

void rally_spawners(GameState& state) {
    const Player* player = GetEntity(state.players, state.active_player);

    if (player == nullptr) return;

    std::vector<Spawner*> closest_spawners = {};

    for (Slot<Spawner>& slot : state.spawners.data) {
        if (!slot.alive || slot.ref.state != SpawnerState::Idle) continue;

        if (closest_spawners.size() < 4) {
            closest_spawners.push_back(&slot.ref);
            continue;
        }

        for (Spawner*& closest_spawner : closest_spawners) {
            if (closest_spawner->position.distance_to(player->position) <
                slot.ref.position.distance_to(player->position))
                continue;

            closest_spawner = &slot.ref;
            break;
        }
    }

    for (Spawner*& spawner : closest_spawners) {
        const Vec2F direction = (player->position - spawner->position).normalized();
        spawner->rally_position = spawner->position + direction * RALLY_DISTANCE;
        spawner->state = SpawnerState::Rallying;
    }
}

void create_spawner(EntityPool<Spawner>& spawners) {
    const Spawner* parent_spawner = get_random_spawner(spawners);

    if (parent_spawner == nullptr) return;

    const float random_x = random_float(MIN_SPAWNER_RANGE, MAX_SPAWNER_RANGE);
    const int x_negative = random_int(0, 1);
    const float random_y = random_float(MIN_SPAWNER_RANGE, MAX_SPAWNER_RANGE);
    const int y_negative = random_int(0, 1);

    Vec2F offset = {.x = random_x, .y = random_y};
    if (x_negative == 1) offset.x = -random_x;
    if (y_negative == 1) offset.y = -random_y;

    CreateEntity(spawners, Spawner{.position = parent_spawner->position + offset, .initial_spawn = 0});
}

void UpdateThreatDirector(GameState& state) {
    ThreatDirector& director = state.threat_director;

    director.spawn_table.clear();

    if (director.threat < 0.1) {
        director.spawn_table.push_back({.enemy = melee_enemy, .weight = 1});
    } else if (director.threat < 0.2) {
        director.spawn_table.push_back({.enemy = melee_enemy, .weight = 0.7});
        director.spawn_table.push_back({.enemy = fast_enemy, .weight = 0.3});
    } else if (director.threat < 0.35) {
        director.spawn_table.push_back({.enemy = melee_enemy, .weight = 0.5});
        director.spawn_table.push_back({.enemy = fast_enemy, .weight = 0.3});
        director.spawn_table.push_back({.enemy = ranged_enemy, .weight = 0.2});
    } else {
        director.spawn_table.push_back({.enemy = melee_enemy, .weight = 0.3});
        director.spawn_table.push_back({.enemy = fast_enemy, .weight = 0.3});
        director.spawn_table.push_back({.enemy = ranged_enemy, .weight = 0.3});
        director.spawn_table.push_back({.enemy = tank_enemy, .weight = 0.1});
    }

    if (!director.threat_active) return;

    const float delta_time = GetFrameTime();

    director.time_to_next_spawner_spread -= delta_time;
    director.time_to_next_rally -= delta_time;

    director.threat += 0.003f * delta_time;

    if (director.time_to_next_spawner_spread <= 0) {
        create_spawner(state.spawners);

        director.time_to_next_spawner_spread = random_float(20, 40);
    }

    if (director.time_to_next_rally <= 0) {
        rally_spawners(state);
        director.time_to_next_rally = random_float(20, 40);
    }

    if (director.threat > 1) director.threat = 1;
}
