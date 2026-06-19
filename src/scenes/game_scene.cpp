#include "scenes/game_scene.hpp"

#include "core/camera.hpp"
#include "core/collision.hpp"
#include "core/entity_pool.hpp"
#include "core/input.hpp"
#include "core/random.hpp"
#include "core/renderer.hpp"
#include "entities/player.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "scenes/game_over_scene.hpp"
#include "scenes/pause_scene.hpp"
#include "systems/enemy_system.hpp"
#include "systems/pickup_system.hpp"
#include "systems/player_system.hpp"
#include "systems/projectile_system.hpp"
#include "systems/scene_manager.hpp"
#include "systems/spawner_system.hpp"
#include "systems/targeting.hpp"

#include "raylib.h"
#include "systems/threat_director.hpp"
#include "systems/tower_system.hpp"
#include <format>

namespace {

void DrawUi(GameState& state) {
    // TOWER COST UI
    const Vec2F tower_center = Vec2F{.x = SCREEN_CENTER.x, .y = SCREEN_HEIGHT - TOWER_SIZE};

    render_rectangle(tower_center, {.x = TOWER_SIZE, .y = TOWER_SIZE}, BLACK, true, true);
    DrawCircle(tower_center.x, tower_center.y, TOWER_SIZE * 0.15, BLUE);

    render_text("Tower", tower_center, 12, BLACK);
    Color cost_color = BLACK;
    if (state.currency < 10) cost_color = RED;
    render_text("10 [RMB]", tower_center + Vec2F{.x = 0, .y = 15}, 12, cost_color);

    // INFO AT TOP
    const std::string difficulty_text = std::format("Difficulty scale: {:.3f}", state.threat_director.threat);
    render_text(difficulty_text, {.x = SCREEN_CENTER.x, .y = 40}, 20, BLACK);

    const std::string currency_text = std::format("Currency: {}", state.currency);
    render_text(currency_text, {.x = SCREEN_CENTER.x, .y = 70}, 20, BLACK);

    // POINTER TO SPAWNER
    Player* player = GetEntity(state.players, state.active_player);
    if (player != nullptr) {
        std::optional<Targetable> closest_spawner = find_closest_target(
            Vec2F{.x = state.camera.target.x, .y = state.camera.target.y}, state.targetables, TARGET_SPAWNER);

        if (closest_spawner.has_value() &&
            !collision_point_rect(
                closest_spawner->position,
                Rect{.position = get_world_position({.x = 0, .y = 0}, state.camera), .size = SCREEN_SIZE})) {

            float distance = player->position.distance_to(closest_spawner->position);
            float angle = player->position.angle_to(closest_spawner->position);

            Vec2F point_a = Vec2F{.x = 200, .y = 0}.rotate(angle) + SCREEN_CENTER;
            Vec2F point_b = Vec2F{.x = 160, .y = -10}.rotate(angle) + SCREEN_CENTER;
            Vec2F point_c = Vec2F{.x = 160, .y = 10}.rotate(angle) + SCREEN_CENTER;

            render_triangle(point_a, point_b, point_c, BLACK);
            render_text(std::format("{}", static_cast<int>(distance)),
                        Vec2F{.x = 130, .y = 0}.rotate(angle) + SCREEN_CENTER, 14, BLACK);
        }
    }
}

void Draw(GameState& state) {
    ClearBackground(GRAY);

    BeginMode2D(state.camera);

    DrawEnemies(state);
    DrawPlayers(state);
    DrawProjectiles(state);
    DrawSpawners(state);
    DrawTowers(state);
    DrawPickups(state);

    DrawSpawnersDebug(state);
    DrawPlayersDebug(state);

    EndMode2D();

    DrawUi(state);
}

void UpdateInputs(GameState& state) {
    if (input_frame.is_key_pressed(Key::Escape)) SCENE_MANAGER.PushScene(state, PAUSE_SCENE);
}
void Update(GameState& state) {
    Player* active_player = GetEntity(state.players, state.active_player);
    if (active_player != nullptr) {
        state.camera.target.x = active_player->position.x;
        state.camera.target.y = active_player->position.y;
    }

    state.targetables = build_targetables(state);

    UpdateInputs(state);
    UpdatePlayers(state);
    UpdateProjectiles(state);
    UpdateEnemies(state);
    UpdateSpawners(state);
    UpdateTowers(state);
    UpdateThreatDirector(state);
    UpdatePickups(state);

    const bool all_enemies_dead = state.enemies.free_indices.size() == state.enemies.data.size() &&
                                  state.spawners.free_indices.size() == state.spawners.data.size();
    // TODO: Better check?
    if (all_enemies_dead || active_player == nullptr) { SCENE_MANAGER.PushScene(state, GAME_OVER_SCENE); }
}

void Destroy(GameState& state) {
    state.Reset();
}

struct InitialSpawnerPlacement {
    Vec2F min_distance;
    Vec2F max_distance;
    int count;
    int initial_enemy_spawn_count;
};

const std::array<InitialSpawnerPlacement, 3> initial_spawner_placements = {{
    InitialSpawnerPlacement{.min_distance = {.x = SCREEN_WIDTH * 0.5f, .y = SCREEN_HEIGHT * 0.5f},
                            .max_distance = {.x = SCREEN_WIDTH, .y = SCREEN_HEIGHT},
                            .count = 3,
                            .initial_enemy_spawn_count = 3},
    InitialSpawnerPlacement{.min_distance = {.x = SCREEN_WIDTH, .y = SCREEN_HEIGHT},
                            .max_distance = {.x = SCREEN_WIDTH * 2, .y = SCREEN_HEIGHT * 2},
                            .count = 5,
                            .initial_enemy_spawn_count = 1},
    InitialSpawnerPlacement{.min_distance = {.x = SCREEN_WIDTH * 1.5f, .y = SCREEN_HEIGHT * 1.5f},
                            .max_distance = {.x = SCREEN_WIDTH * 3, .y = SCREEN_HEIGHT * 3},
                            .count = 8,
                            .initial_enemy_spawn_count = 0},
}};

void Init(GameState& state) {
    state.Reset();

    for (InitialSpawnerPlacement placement : initial_spawner_placements) {
        for (int i = 0; i < placement.count; i++) {
            bool negative_x = random_int(0, 1) == 1;
            bool negative_y = random_int(0, 1) == 1;
            float random_x = random_float(placement.min_distance.x, placement.max_distance.x);
            float random_y = random_float(placement.min_distance.y, placement.max_distance.y);

            if (negative_x) random_x = -random_x;
            if (negative_y) random_y = -random_y;

            CreateEntity(state.spawners, {.position = {.x = random_x, .y = random_y},
                                          .angle = random_float(0, 359),
                                          .spawn_amount = 1,
                                          .initial_spawn = placement.initial_enemy_spawn_count});
        }
    }

    Player player = {.position = SCREEN_CENTER};
    state.active_player = CreateEntity(state.players, player);
    state.camera.target.x = player.position.x;
    state.camera.target.y = player.position.y;
}

} // namespace

const Scene GAME_SCENE = {.name = "Game", .init = Init, .update = Update, .draw = Draw, .destroy = Destroy};
