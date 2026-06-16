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

#include "raylib.h"
#include "systems/threat_director.hpp"
#include "systems/tower_system.hpp"
#include <algorithm>
#include <format>

namespace {

const ParticleTemplate MUZZLE_SMOKE_PARTICLE = ParticleTemplate({
    .speed = {.start = {.min = 60, .max = 80}, .end = {.min = 5, .max = 20}},
    .size = {.start = {.min = 2, .max = 4}, .end = {.min = 0, .max = 1}},
    .color = {.start = WHITE, .end = Color(255, 255, 255, 0)},
    .lifetime = {.min = 0.1f, .max = 0.3f},
});
Emitter MUZZLE_SMOKE_EMITTER = Emitter{.position = {.x = 0, .y = 0},
                                       .direction = {.x = 0, .y = 0},
                                       .spread = 40,
                                       .particle_template = MUZZLE_SMOKE_PARTICLE,
                                       .rate = 0,
                                       .duration = 0,
                                       .burst = 30};

ParticleTemplate MUZZLE_FLASH_PARTICLE = ParticleTemplate({
    .display = {.type = ParticleDisplayType::Sprite, .sprite_info = {"muzzle_flash", {.x = 16, .y = 16}}},
    .speed = {.start = {.min = 0, .max = 0}, .end = {.min = 0, .max = 0}},
    .size = {.start = {.min = 32, .max = 32}, .end = {.min = 32, .max = 32}},
    .color = {.start = WHITE, .end = WHITE},
    .lifetime = {.min = 0.03f, .max = 0.08f},
});
Emitter MUZZLE_FLASH_EMITTER = Emitter{.position = {.x = 0, .y = 0},
                                       .direction = {.x = 0, .y = 0},
                                       .spread = 5,
                                       .particle_template = MUZZLE_FLASH_PARTICLE,
                                       .rate = 0,
                                       .duration = 0,
                                       .burst = 1};

ParticleSystem particles{};

void DrawUi(GameState& state) {
    // TOWER COST UI
    const Vec2F tower_center = Vec2F{.x = SCREEN_CENTER.x, .y = SCREEN_HEIGHT - 50};

    render_rectangle(tower_center, {.x = TOWER_SIZE, .y = TOWER_SIZE}, BLACK, true, true);
    DrawCircle(tower_center.x, tower_center.y, TOWER_SIZE * 0.15, BLUE);

    render_text("Tower", tower_center, 12, BLACK);
    Color cost_color = BLACK;
    if (state.currency < 10) cost_color = RED;
    render_text("10 [RMB]", tower_center + Vec2F{.x = 0, .y = 15}, 12, cost_color);

    // INFO AT TOP
    const std::string difficulty_text = std::format("Difficulty scale: {}", state.threat_director.threat);
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
            // TODO: Get this from the pistol?
            auto barrel_end_pos = active_player->position + (direction * 30);
            CreateEntity(state.projectiles, Projectile{.direction = direction,
                                                       .position = barrel_end_pos,
                                                       .life_time = 2.0,
                                                       .damage = active_player->damage,
                                                       .flags = TARGET_SPAWNER | TARGET_ENEMY});
            active_player->time_since_last_shot = 0;

            MUZZLE_FLASH_PARTICLE.rotation = direction.angle();
            MUZZLE_FLASH_PARTICLE.display.sprite_info.frame = random_int(0, 3);
            MUZZLE_FLASH_EMITTER.particle_template = MUZZLE_FLASH_PARTICLE;
            MUZZLE_FLASH_EMITTER.position = Vec2F{.x = barrel_end_pos.x, .y = barrel_end_pos.y};
            particles.play(MUZZLE_FLASH_EMITTER);

            // MUZZLE_SMOKE_EMITTER.position = Vec2F{.x = barrel_end_pos.x, .y = barrel_end_pos.y};
            // MUZZLE_SMOKE_EMITTER.direction = Vec2F{.x = direction.x, .y = direction.y};
            // particles.play(MUZZLE_SMOKE_EMITTER);

            PlaySound(get_sound("pistol"));
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
    particles.update(GetFrameTime());
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

const std::array<InitialSpawnerPlacement, 3> initial_spawner_placements = {
    {InitialSpawnerPlacement{.min_distance = {.x = SCREEN_WIDTH * 0.5f, .y = SCREEN_HEIGHT * 0.5f},
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
    state.camera.target.x = player.position.x;
    state.camera.target.y = player.position.y;
}

} // namespace

const Scene GAME_SCENE = {.name = "Game", .init = Init, .update = Update, .draw = Draw, .destroy = Destroy};
