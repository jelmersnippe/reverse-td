#include "scenes/game_scene.hpp"
#include "core/entity_pool.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raylib.h"
#include "systems/enemy_system.hpp"
#include "systems/player_system.hpp"
#include "systems/projectile_system.hpp"
#include "systems/scene_manager.hpp"
#include "systems/spawner_system.hpp"
#include "systems/targeting.hpp"

#include "raymath.h"
#include "systems/threat_director.hpp"
#include "systems/tower_system.hpp"
#include <algorithm>
#include <format>
#include <iostream>

namespace {

void Draw(const GameState& state) {
    ClearBackground(WHITE);

    BeginMode2D(state.camera);

    DrawEnemies(state.enemies);
    DrawPlayers(state.players);
    DrawProjectiles(state.projectiles);
    DrawSpawners(state.spawners);
    DrawTowers(state.towers, state.camera);

    EndMode2D();

    const std::string difficulty_text = std::format("Difficulty scale: {}", state.threat_director.threat);
    const int difficulty_text_width = MeasureText(difficulty_text.c_str(), 20);
    DrawText(difficulty_text.c_str(), SCREEN_WIDTH / 2 - difficulty_text_width / 2, 30, 20, BLACK);

    const std::string currency_text = std::format("Currency: {}", state.currency);
    const int currency_text_width = MeasureText(currency_text.c_str(), 20);
    DrawText(currency_text.c_str(), SCREEN_WIDTH / 2 - currency_text_width / 2, 60, 20, BLACK);
}

void UpdateInputs(GameState& state) {
    const Vector2 mouse_position = GetScreenToWorld2D(GetMousePosition(), state.camera);
    Player* active_player = GetEntity(state.players, state.active_player);

    if (active_player == nullptr) {
        state.inputs.clear();
        return;
    };

    for (const Input& input : state.inputs) {
        switch (input) {
            case Input::LeftMouse: {
                if (active_player->time_since_last_shot < TIME_BETWEEN_SHOTS) break;

                const Vector2 direction =
                    Vector2Subtract(GetScreenToWorld2D(GetMousePosition(), state.camera), active_player->position);
                CreateEntity(state.projectiles, Projectile{.velocity = Vector2Normalize(direction) * PROJECTILE_SPEED,
                                                           .position = active_player->position,
                                                           .life_time = 2.0,
                                                           .damage = active_player->damage,
                                                           .flags = TARGET_SPAWNER | TARGET_ENEMY});
                active_player->time_since_last_shot = 0;
                break;
            }
            case Input::X: {
                for (size_t i = 0; i < state.towers.data.size(); i++) {
                    Slot<Tower>& slot = state.towers.data[i];
                    if (!slot.alive) continue;

                    const bool is_hovered =
                        CheckCollisionPointRec(mouse_position, {.x = slot.ref.position.x - TOWER_SIZE / 2,
                                                                .y = slot.ref.position.y - TOWER_SIZE / 2,
                                                                .width = TOWER_SIZE,
                                                                .height = TOWER_SIZE});

                    if (!is_hovered || Vector2Distance(active_player->position, slot.ref.position) > PLAYER_RANGE)
                        continue;

                    DestroyEntity(state.towers, EntityHandle{.index = i, .generation = slot.generation});
                    state.currency += GetScrapValue(slot.ref);
                }
                break;
            }
            case Input::RightMouse: {
                if (state.currency < TOWER_COST) break;

                if (std::ranges::any_of(state.towers.data, [mouse_position](const Slot<Tower>& tower_ref) {
                        return tower_ref.alive && CheckCollisionRecs({.x = mouse_position.x - TOWER_SIZE / 2,
                                                                      .y = mouse_position.y - TOWER_SIZE / 2,
                                                                      .width = TOWER_SIZE,
                                                                      .height = TOWER_SIZE},
                                                                     {.x = tower_ref.ref.position.x - TOWER_SIZE / 2,
                                                                      .y = tower_ref.ref.position.y - TOWER_SIZE / 2,
                                                                      .width = TOWER_SIZE,
                                                                      .height = TOWER_SIZE});
                    })) {
                    break;
                }
                if (std::ranges::any_of(state.spawners.data, [mouse_position](const Slot<Spawner>& spawner_ref) {
                        return spawner_ref.alive &&
                               CheckCollisionRecs({.x = mouse_position.x - TOWER_SIZE / 2,
                                                   .y = mouse_position.y - TOWER_SIZE / 2,
                                                   .width = TOWER_SIZE,
                                                   .height = TOWER_SIZE},
                                                  {.x = spawner_ref.ref.position.x - SPAWNER_SIZE / 2,
                                                   .y = spawner_ref.ref.position.y - SPAWNER_SIZE / 2,
                                                   .width = SPAWNER_SIZE,
                                                   .height = SPAWNER_SIZE});
                    })) {
                    break;
                }
                if (std::ranges::any_of(state.players.data, [mouse_position](const Slot<Player>& player_ref) {
                        return player_ref.alive && CheckCollisionRecs({.x = mouse_position.x - TOWER_SIZE / 2,
                                                                       .y = mouse_position.y - TOWER_SIZE / 2,
                                                                       .width = TOWER_SIZE,
                                                                       .height = TOWER_SIZE},
                                                                      {.x = player_ref.ref.position.x - PLAYER_SIZE / 2,
                                                                       .y = player_ref.ref.position.y - PLAYER_SIZE / 2,
                                                                       .width = PLAYER_SIZE,
                                                                       .height = PLAYER_SIZE});
                    })) {
                    break;
                }
                if (std::ranges::any_of(state.enemies.data, [mouse_position](const Slot<Enemy>& enemy_ref) {
                        return enemy_ref.alive &&
                               CheckCollisionPointRec(enemy_ref.ref.position, {.x = mouse_position.x - TOWER_SIZE / 2,
                                                                               .y = mouse_position.y - TOWER_SIZE / 2,
                                                                               .width = TOWER_SIZE,
                                                                               .height = TOWER_SIZE});
                    })) {
                    break;
                }

                CreateEntity(state.towers, Tower{.position = mouse_position});
                state.currency -= TOWER_COST;
                break;
            }
            case Input::W:
                active_player->direction += {.x = 0, .y = -1};
                break;
            case Input::S:
                active_player->direction += {.x = 0, .y = 1};
                break;
            case Input::A:
                active_player->direction += {.x = -1, .y = 0};
                break;
            case Input::D:
                active_player->direction += {.x = 1, .y = 0};
                break;
            default:
                throw "Unrecognized input";
        }
    }

    state.inputs.clear();
}
void Update(GameState& state) {
    Player* active_player = GetEntity(state.players, state.active_player);
    if (active_player != nullptr) state.camera.target = {active_player->position};

    UpdateInputs(state);
    UpdatePlayers(state);
    UpdateProjectiles(state);
    UpdateEnemies(state);
    UpdateSpawners(state);
    UpdateTowers(state);
    UpdateThreatDirector(state);
}

void Destroy(GameState& state) {
    state.Reset();
}

void Init(GameState& state) {
    Player player = {.position = Vector2{.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2}};
    state.active_player = CreateEntity(state.players, player);
    state.camera.target = {player.position};

    CreateEntity(state.spawners, {.position = {.x = -200, .y = 200}, .spawn_amount = 2, .initial_spawn = 2});
    CreateEntity(state.spawners, {.position = {.x = 1200, .y = -400}});
    CreateEntity(state.spawners, {.position = {.x = 600, .y = 800}, .initial_spawn = 2});
    CreateEntity(state.spawners, {.position = {.x = 0, .y = 1200}, .initial_spawn = 1});
}

} // namespace

const Scene GAME_SCENE = {.name = "Game", .init = Init, .update = Update, .draw = Draw, .destroy = Destroy};
