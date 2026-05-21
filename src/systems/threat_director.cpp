#include "threat_director.hpp"
#include "entities/enemy.hpp"
#include "game_state.hpp"
#include "raylib.h"

void UpdateThreatDirector(GameState& state) {
    ThreatDirector& director = state.threat_director;
    const float delta_time = GetFrameTime();

    director.threat += 0.3f * delta_time;

    director.spawn_table.clear();

    if (director.threat < 10) {
        director.spawn_table.push_back({.enemy = melee_enemy, .weight = 1});
    } else if (director.threat < 20) {
        director.spawn_table.push_back({.enemy = melee_enemy, .weight = 0.7});
        director.spawn_table.push_back({.enemy = fast_enemy, .weight = 0.3});
    } else if (director.threat < 35) {
        director.spawn_table.push_back({.enemy = melee_enemy, .weight = 0.5});
        director.spawn_table.push_back({.enemy = fast_enemy, .weight = 0.3});
        director.spawn_table.push_back({.enemy = ranged_enemy, .weight = 0.2});
    } else {
        director.spawn_table.push_back({.enemy = melee_enemy, .weight = 0.3});
        director.spawn_table.push_back({.enemy = fast_enemy, .weight = 0.3});
        director.spawn_table.push_back({.enemy = ranged_enemy, .weight = 0.3});
        director.spawn_table.push_back({.enemy = tank_enemy, .weight = 0.1});
    }
}
