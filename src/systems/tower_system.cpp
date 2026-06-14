#include "tower_system.hpp"

#include "core/asset_manager.hpp"
#include "core/entity_pool.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raylib.h"
#include "raymath.h"
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

    if (Vector2Distance(tower.position, target->position) > tower.range) return;

    // Attacking time
    if (tower.time_since_last_attack >= 60.0 / tower.fire_rate) {
        const Vector2 direction = target->position - tower.position;
        CreateEntity(state.projectiles, {.direction = Vector2Normalize(direction),
                                         .position = tower.position,
                                         .life_time = 2.0,
                                         .damage = tower.damage,
                                         .flags = TARGET_ENEMY | TARGET_SPAWNER});
        tower.time_since_last_attack = 0;
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
    return default_scrap_value * modifier;
}

void DrawTowers(const EntityPool<Tower>& towers, const Camera2D& camera) {
    const Vector2 mouse_position = GetScreenToWorld2D(GetMousePosition(), camera);
    for (const Slot<Tower>& tower : towers.data) {
        if (!tower.alive) continue;

        const Vector2 tower_top_left = {.x = tower.ref.position.x - TOWER_SIZE / 2,
                                        .y = tower.ref.position.y - TOWER_SIZE / 2};
        DrawTextureEx(get_sprite("turret"), tower_top_left, 0, 4, WHITE);

        if (tower.ref.scrapping) {
            DrawRectangle(tower.ref.position.x - 15, tower.ref.position.y + 10, 30, 5, BLACK);
            DrawRectangle(tower.ref.position.x - 15, tower.ref.position.y + 10,
                          30 * ((float)tower.ref.scrap_time / (float)tower.ref.time_to_scrap), 5, WHITE);

            const char* scrap_text = "Scrapping..";
            int scrap_text_width = MeasureText(scrap_text, 12);
            DrawText(scrap_text, tower.ref.position.x - scrap_text_width / 2, tower.ref.position.y + 15, 12, BLACK);
        } else {
            const bool is_hovered = CheckCollisionPointRec(
                mouse_position,
                {.x = tower_top_left.x, .y = tower_top_left.y, .width = TOWER_SIZE, .height = TOWER_SIZE});

            if (is_hovered && Vector2Distance(camera.target, tower.ref.position) < PLAYER_RANGE) {
                std::string scrap_text = std::format("[x] Scrap for: {}", GetScrapValue(tower.ref));
                int scrap_text_width = MeasureText(scrap_text.c_str(), 12);
                DrawText(scrap_text.c_str(), tower.ref.position.x - scrap_text_width / 2, tower.ref.position.y + 10, 12,
                         BLACK);
            }
        }

        DrawHealth(tower.ref.position - Vector2{.x = 0, .y = TOWER_SIZE / 2 + 10}, tower.ref.health);
    }
}
