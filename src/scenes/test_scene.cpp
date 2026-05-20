#include "scenes/test_scene.hpp"
#include "core/entity_pool.hpp"
#include "entities/enemy.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raylib.h"
#include "systems/enemy_system.hpp"
#include "systems/projectile_system.hpp"
#include "systems/scene_manager.hpp"
#include "systems/spawner_system.hpp"
#include "systems/targeting.hpp"

#include "raymath.h"
#include "systems/tower_system.hpp"
#include <format>

namespace {
void Init(GameState& state) {
    state.camera.target = Vector2{.x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 2};
}

void Draw(const GameState& state) {
    ClearBackground(WHITE);

    DrawText(std::format("Personal space: {}", PERSONAL_SPACE).c_str(), 20, 50, 12, BLACK);

    BeginMode2D(state.camera);

    for (const Slot<Enemy>& enemy : state.enemies.data) {
        if (!enemy.alive) continue;

        DrawCircle(enemy.ref.position.x, enemy.ref.position.y, enemy.ref.size, enemy.ref.color);
    }

    for (const Slot<Projectile>& projectile : state.projectiles.data) {
        if (!projectile.alive) continue;

        DrawCircle(projectile.ref.position.x, projectile.ref.position.y, PROJECTILE_SIZE, RED);
    }

    EndMode2D();
}

void Update(GameState& state) {
    if (IsKeyDown(KEY_F5) && PERSONAL_SPACE > 0) PERSONAL_SPACE -= 1;
    if (IsKeyDown(KEY_F6)) PERSONAL_SPACE += 1;

    const Vector2 destination = GetScreenToWorld2D(GetMousePosition(), state.camera);

    for (Input input : state.inputs) {
        switch (input) {
            case Input::One: {
                Enemy new_enemy = melee_enemy;
                new_enemy.position = destination;
                new_enemy.damage = 0;
                CreateEntity(state.enemies, new_enemy);
                break;
            }
            case Input::Two: {
                Enemy new_enemy = fast_enemy;
                new_enemy.position = destination;
                new_enemy.damage = 0;
                CreateEntity(state.enemies, new_enemy);
                break;
            }
            case Input::Three: {
                Enemy new_enemy = ranged_enemy;
                new_enemy.position = destination;
                new_enemy.damage = 0;
                CreateEntity(state.enemies, new_enemy);
                break;
            }
            case Input::Four: {
                Enemy new_enemy = tank_enemy;
                new_enemy.position = destination;
                new_enemy.damage = 0;
                CreateEntity(state.enemies, new_enemy);
                break;
            }
            default:
                break;
        }
    }

    state.inputs.clear();

    UpdateEnemies(state);
    UpdateProjectiles(state);
}

void Destroy(GameState& state) {
    state.Reset();
}

} // namespace

const Scene TEST_SCENE = {.name = "Test", .init = Init, .update = Update, .draw = Draw, .destroy = Destroy};
