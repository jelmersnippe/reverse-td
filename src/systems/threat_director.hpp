#pragma once

#include "entities/enemy.hpp"
struct GameState;

void UpdateThreatDirector(GameState& state);

struct SpawnOption {
    Enemy enemy;
    float weight;
};

struct ThreatDirector {
    float threat = 0;

    float time_to_next_spawner_spread = 30;
    float time_to_next_rally = 5;

    std::vector<SpawnOption> spawn_table = {{.enemy = melee_enemy, .weight = 1}};
};
