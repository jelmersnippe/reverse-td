#include "scenes/game_scene.hpp"

#include "core/camera.hpp"
#include "core/collision.hpp"
#include "core/entity_pool.hpp"
#include "core/input.hpp"
#include "core/particles.hpp"
#include "core/random.hpp"
#include "core/renderer.hpp"
#include "entities/player.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "scenes/pause_scene.hpp"
#include "systems/enemy_system.hpp"
#include "systems/pickup_system.hpp"
#include "systems/player_system.hpp"
#include "systems/projectile_system.hpp"
#include "systems/scene_manager.hpp"
#include "systems/spawner_system.hpp"
#include "systems/targeting.hpp"

#include "systems/threat_director.hpp"
#include "systems/tower_system.hpp"
#include <algorithm>
#include <format>

namespace {

const ParticleTemplate MUZZLE_SMOKE_PARTICLE = ParticleTemplate({
    .speed = {.start = {.min = 20, .max = 80}, .end = {.min = 5, .max = 20}},
    .size = {.start = {.min = 3, .max = 8}, .end = {.min = 12, .max = 24}},
    .color = {.start = Color(120, 120, 120, 150), .end = Color(80, 80, 80, 0)},
    .lifetime = {.min = 0.2f, .max = 0.5f},
});
Emitter MUZZLE_SMOKE_EMITTER = Emitter{.position = {.x = 0, .y = 0},
                                       .direction = {.x = 0, .y = 0},
                                       .spread = 40,
                                       .particle_template = MUZZLE_SMOKE_PARTICLE,
                                       .rate = 0,
                                       .duration = 0,
                                       .burst = 30};

const ParticleTemplate MUZZLE_FLASH_PARTICLE = ParticleTemplate({
    .speed = {.start = {.min = 150, .max = 300}, .end = {.min = 50, .max = 100}},
    .size = {.start = {.min = 14, .max = 20}, .end = {.min = 0, .max = 2}},
    .color = {.start = Color(255, 255, 180, 255), .end = Color(255, 100, 20, 0)},
    .lifetime = {.min = 0.03f, .max = 0.08f},
});
Emitter MUZZLE_FLASH_EMITTER = Emitter{.position = {.x = 0, .y = 0},
                                       .direction = {.x = 0, .y = 0},
                                       .spread = 20,
                                       .particle_template = MUZZLE_FLASH_PARTICLE,
                                       .rate = 0,
                                       .duration = 0,
                                       .burst = 10};

ParticleSystem particles{};

void DrawUi(GameState& state) {
    // TOWER COST UI
    const Vec2F tower_center = Vec2F{.x = SCREEN_CENTER.x, .y = SCREEN_HEIGHT - 50};
    const Vec2F tower_top_left = tower_center - Vec2F{.x = TOWER_SIZE / 4, .y = TOWER_SIZE / 4};

    render_rectangle(tower_center, {.x = TOWER_SIZE, .y = TOWER_SIZE}, BLACK, true);
    DrawCircle(tower_center.x, tower_center.y, TOWER_SIZE * 0.15, BLUE);

    const int tower_text_width = MeasureText("Tower", 12);
    const int cost_text_width = MeasureText("10 [RMB]", 12);
    DrawText("Tower", tower_center.x - tower_text_width / 2, tower_top_left.y - 12, 12, BLACK);
    Color cost_color = BLACK;
    if (state.currency < 10) cost_color = RED;
    DrawText("10 [RMB]", SCREEN_CENTER.x - cost_text_width / 2, tower_top_left.y + TOWER_SIZE / 2 + 8, 12, cost_color);

    // INFO AT TOP
    const std::string difficulty_text = std::format("Difficulty scale: {}", state.threat_director.threat);
    const int difficulty_text_width = MeasureText(difficulty_text.c_str(), 20);
    DrawText(difficulty_text.c_str(), SCREEN_WIDTH / 2 - difficulty_text_width / 2, 30, 20, BLACK);

    const std::string currency_text = std::format("Currency: {}", state.currency);
    const int currency_text_width = MeasureText(currency_text.c_str(), 20);
    DrawText(currency_text.c_str(), SCREEN_WIDTH / 2 - currency_text_width / 2, 60, 20, BLACK);

    // POINTER TO SPAWNER
    Player* player = GetEntity(state.players, state.active_player);
    if (player != nullptr) {
        std::optional<Targetable> closest_spawner =
            find_closest_target(Vec2F{state.camera.target.x, state.camera.target.y}, state.targetables, TARGET_SPAWNER);

        if (closest_spawner.has_value() &&
            !collision_point_rect(
                closest_spawner->position,
                Rect{.position = get_world_position({.x = 0, .y = 0}, state.camera), .size = SCREEN_SIZE})) {
            float angle = player->position.angle_to(closest_spawner->position);

            Vec2F point_a = Vec2F{.x = 200, .y = 0}.rotate_to(angle) + SCREEN_CENTER;
            Vec2F point_b = Vec2F{.x = 160, .y = -10}.rotate_to(angle) + SCREEN_CENTER;
            Vec2F point_c = Vec2F{.x = 160, .y = 10}.rotate_to(angle) + SCREEN_CENTER;

            render_triangle(point_a, point_b, point_c, BLACK);
        }
    }
}

void Draw(GameState& state) {
    ClearBackground(GRAY);

    BeginMode2D(state.camera);

    DrawEnemies(state.enemies);
    DrawPlayers(state.players, state.camera);
    DrawProjectiles(state.projectiles);
    DrawSpawners(state.spawners);
    DrawTowers(state.towers, state.camera);
    DrawPickups(state.pickups);

    particles.draw();

    EndMode2D();

    DrawUi(state);
}

void build_tower(GameState& state, Vec2F position) {
    if (state.currency < TOWER_COST) return;

    const Vec2F to_place_top_left = {.x = position.x - TOWER_SIZE / 2, .y = position.y - TOWER_SIZE / 2};
    const Vec2F tower_size = {.x = TOWER_SIZE, .y = TOWER_SIZE};

    if (std::ranges::any_of(state.towers.data, [position](const Slot<Tower>& tower_ref) {
            return tower_ref.alive && CheckCollisionRecs({.x = position.x - TOWER_SIZE / 2,
                                                          .y = position.y - TOWER_SIZE / 2,
                                                          .width = TOWER_SIZE,
                                                          .height = TOWER_SIZE},
                                                         {.x = tower_ref.ref.position.x - TOWER_SIZE / 2,
                                                          .y = tower_ref.ref.position.y - TOWER_SIZE / 2,
                                                          .width = TOWER_SIZE,
                                                          .height = TOWER_SIZE});
        })) {
        return;
    }
    if (std::ranges::any_of(state.spawners.data, [position](const Slot<Spawner>& spawner_ref) {
            return spawner_ref.alive && CheckCollisionRecs({.x = position.x - TOWER_SIZE / 2,
                                                            .y = position.y - TOWER_SIZE / 2,
                                                            .width = TOWER_SIZE,
                                                            .height = TOWER_SIZE},
                                                           {.x = spawner_ref.ref.position.x - SPAWNER_SIZE / 2,
                                                            .y = spawner_ref.ref.position.y - SPAWNER_SIZE / 2,
                                                            .width = SPAWNER_SIZE,
                                                            .height = SPAWNER_SIZE});
        })) {
        return;
    }
    if (std::ranges::any_of(state.players.data, [position](const Slot<Player>& player_ref) {
            return player_ref.alive && CheckCollisionRecs({.x = position.x - TOWER_SIZE / 2,
                                                           .y = position.y - TOWER_SIZE / 2,
                                                           .width = TOWER_SIZE,
                                                           .height = TOWER_SIZE},
                                                          {.x = player_ref.ref.position.x - PLAYER_SIZE / 2,
                                                           .y = player_ref.ref.position.y - PLAYER_SIZE / 2,
                                                           .width = PLAYER_SIZE,
                                                           .height = PLAYER_SIZE});
        })) {
        return;
    }
    if (std::ranges::any_of(state.enemies.data, [to_place_top_left, tower_size](const Slot<Enemy>& enemy_ref) {
            return enemy_ref.alive &&
                   collision_point_rect(enemy_ref.ref.position, {.position = to_place_top_left, .size = tower_size});
        })) {
        return;
    }

    CreateEntity(state.towers, Tower{.position = position});
    state.currency -= TOWER_COST;
}

void UpdateInputs(GameState& state) {
    const Vec2F mouse_position = get_mouse_world_position(state.camera);
    Player* active_player = GetEntity(state.players, state.active_player);

    if (input_frame.is_key_pressed(Key::X)) {
        for (auto& slot : state.towers.data) {
            if (!slot.alive) continue;

            if (slot.ref.scrapping) continue;

            const bool is_hovered =
                collision_point_rect(mouse_position, {.position =
                                                          {
                                                              .x = slot.ref.position.x - TOWER_SIZE / 2,
                                                              .y = slot.ref.position.y - TOWER_SIZE / 2,
                                                          },
                                                      .size = {.x = TOWER_SIZE, .y = TOWER_SIZE}});

            if (!is_hovered || active_player->position.distance_to(slot.ref.position) > PLAYER_RANGE) continue;

            slot.ref.scrapping = true;
        }
    }
    if (input_frame.is_key_pressed(Key::Escape)) SCENE_MANAGER.PushScene(state, PAUSE_SCENE);
    if (input_frame.is_mouse_down(Mouse::Left)) {
        if (active_player->time_since_last_shot >= TIME_BETWEEN_SHOTS) {
            const Vec2F direction =
                active_player->position.direction_to(get_mouse_world_position(state.camera)).normalized();
            auto barrel_end_pos = active_player->position + (direction * 30);
            CreateEntity(state.projectiles, Projectile{.direction = direction,
                                                       .position = barrel_end_pos,
                                                       .life_time = 2.0,
                                                       .damage = active_player->damage,
                                                       .flags = TARGET_SPAWNER | TARGET_ENEMY});
            active_player->time_since_last_shot = 0;

            MUZZLE_FLASH_EMITTER.position = Vec2F{.x = barrel_end_pos.x, .y = barrel_end_pos.y};
            MUZZLE_FLASH_EMITTER.direction = Vec2F{.x = direction.x, .y = direction.y};
            particles.play(MUZZLE_FLASH_EMITTER);

            MUZZLE_SMOKE_EMITTER.position = Vec2F{.x = barrel_end_pos.x, .y = barrel_end_pos.y};
            MUZZLE_SMOKE_EMITTER.direction = Vec2F{.x = direction.x, .y = direction.y};
            particles.play(MUZZLE_SMOKE_EMITTER);
        }
    }

    if (input_frame.is_mouse_pressed(Mouse::Right)) { build_tower(state, mouse_position); }

    if (input_frame.is_key_down(Key::W)) active_player->direction += {.x = 0, .y = -1};
    if (input_frame.is_key_down(Key::S)) active_player->direction += {.x = 0, .y = 1};
    if (input_frame.is_key_down(Key::A)) active_player->direction += {.x = -1, .y = 0};
    if (input_frame.is_key_down(Key::D)) active_player->direction += {.x = 1, .y = 0};
}
void Update(GameState& state) {
    Player* active_player = GetEntity(state.players, state.active_player);
    if (active_player != nullptr)
        state.camera.target = Vector2{.x = active_player->position.x, .y = active_player->position.y};

    state.targetables = build_targetables(state);

    UpdateInputs(state);
    UpdatePlayers(state);
    UpdateProjectiles(state);
    UpdateEnemies(state);
    UpdateSpawners(state);
    UpdateTowers(state);
    UpdateThreatDirector(state);
    UpdatePickups(state);
    particles.update(GetFrameTime());
}

void Destroy(GameState& state) {
    state.Reset();
}

struct InitialSpawnerPlacement {
    Vector2 min_distance;
    Vector2 max_distance;
    int count;
    int initial_enemy_spawn_count;
};

const std::array<InitialSpawnerPlacement, 3> initial_spawner_placements = {
    {InitialSpawnerPlacement{.min_distance = {.x = SCREEN_WIDTH, .y = SCREEN_HEIGHT},
                             .max_distance = {.x = SCREEN_WIDTH * 2, .y = SCREEN_HEIGHT * 2},
                             .count = 3,
                             .initial_enemy_spawn_count = 3},
     InitialSpawnerPlacement{.min_distance = {.x = SCREEN_WIDTH * 2, .y = SCREEN_HEIGHT * 2},
                             .max_distance = {.x = SCREEN_WIDTH * 4, .y = SCREEN_HEIGHT * 4},
                             .count = 5,
                             .initial_enemy_spawn_count = 1},
     InitialSpawnerPlacement{.min_distance = {.x = SCREEN_WIDTH * 3, .y = SCREEN_HEIGHT * 3},
                             .max_distance = {.x = SCREEN_WIDTH * 6, .y = SCREEN_HEIGHT * 6},
                             .count = 8,
                             .initial_enemy_spawn_count = 0}},
};

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
                                          .spawn_amount = 1,
                                          .initial_spawn = placement.initial_enemy_spawn_count});
        }
    }

    Player player = {.position = SCREEN_CENTER};
    state.active_player = CreateEntity(state.players, player);
    state.camera.target = Vector2{.x = player.position.x, .y = player.position.y};
}

} // namespace

const Scene GAME_SCENE = {.name = "Game", .init = Init, .update = Update, .draw = Draw, .destroy = Destroy};
