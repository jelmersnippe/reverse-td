#include "tower_system.hpp"

#include "core/camera.hpp"
#include "core/collision.hpp"
#include "core/entity_pool.hpp"
#include "core/renderer.hpp"
#include "core/sound.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raylib.h"
#include "systems/targeting.hpp"
#include <format>

void Update(Slot<Tower>& slot, GameState& state) {
    Tower& tower = slot.ref;
    const float delta_time = GetFrameTime();

    tower.time_since_last_attack += delta_time;

    if (tower.scrapping) {
        tower.scrap_time += delta_time;

        if (tower.scrap_time >= tower.time_to_scrap) {
            DestroyEntity(state.towers, slot.handle);
            state.currency += GetScrapValue(slot.ref);
            return;
        }
    }

    // TODO: Store in Tower and only update every x frames / when out of range
    std::optional<Targetable> target =
        find_closest_target(tower.position, state.targetables, TARGET_ENEMY | TARGET_SPAWNER);

    if (!target.has_value()) return;

    if (tower.position.distance_to(target->position) > tower.range) return;

    tower.target_position = Vec2F{.x = target->position.x, .y = target->position.y};

    // Attacking time
    if (tower.time_since_last_attack >= 60.0 / tower.fire_rate) {
        const Vec2F direction = target->position - tower.position;
        CreateEntity(state.projectiles, {.direction = direction.normalized(),
                                         .position = tower.position,
                                         .life_time = 2.0,
                                         .damage = tower.damage,
                                         .flags = TARGET_ENEMY | TARGET_SPAWNER});
        tower.time_since_last_attack = 0;

        play_sound("tower_shot");
    }
}

void UpdateTowers(GameState& state) {
    for (Slot<Tower>& tower : state.towers.data) {
        if (!tower.alive) continue;

        Update(tower, state);
    }
}

int GetScrapValue(const Tower& tower) {
    const float default_scrap_value = (TOWER_COST * 0.9);
    const float modifier = (float)tower.health.current / (float)tower.health.max;
    return std::floor(default_scrap_value * modifier);
}

void DrawTowers(const EntityPool<Tower>& towers, const Camera2D& camera) {
    const Vec2F mouse_position = get_mouse_world_position(camera);
    const Vec2F tower_size = Vec2F{.x = TOWER_SIZE, .y = TOWER_SIZE};
    for (const Slot<Tower>& tower : towers.data) {
        if (!tower.alive) continue;

        render_sprite({"turret", {16, 16}}, tower.ref.position, tower_size,
                      tower.ref.position.angle_to(Vec2F{tower.ref.target_position}) + 90);

        if (tower.ref.scrapping) {
            const Vec2F scrap_bar_top_left = {.x = tower.ref.position.x - 15, .y = tower.ref.position.y + 10};
            render_rectangle(scrap_bar_top_left, {.x = 30, .y = 5}, BLACK, false);
            render_rectangle(scrap_bar_top_left,
                             {.x = 30 * ((float)tower.ref.scrap_time / (float)tower.ref.time_to_scrap), .y = 5}, WHITE,
                             false);

            render_text("Scrapping..", tower.ref.position + Vec2F{0, 20}, 12, BLACK);
        } else {
            const Vec2F tower_top_left = {.x = tower.ref.position.x - TOWER_SIZE / 2,
                                          .y = tower.ref.position.y - TOWER_SIZE / 2};
            const bool is_hovered =
                collision_point_rect(mouse_position, {.position = tower_top_left, .size = tower_size});

            if (is_hovered &&
                Vec2F{.x = camera.target.x, .y = camera.target.y}.distance_to(tower.ref.position) < PLAYER_RANGE) {
                std::string scrap_text = std::format("[x] Scrap for: {}", GetScrapValue(tower.ref));
                render_text(scrap_text, tower.ref.position + Vec2F{.x = 0, .y = 20}, 12, BLACK);
            }
        }

        DrawHealth(tower.ref.position - Vec2F{.x = 0, .y = TOWER_SIZE / 2 + 10}, tower.ref.health);
    }
}
