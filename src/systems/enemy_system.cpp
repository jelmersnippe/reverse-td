#include "enemy_system.hpp"

#include "array"
#include "core/asset_manager.hpp"
#include "core/entity_pool.hpp"
#include "entities/enemy.hpp"
#include "entities/projectile.hpp"
#include "entities/spawner.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raylib.h"
#include "raymath.h"
#include "systems/targeting.hpp"
#include <optional>

Vector2 get_separation_direction(Enemy& enemy, Targetable& target, GameState& state) {
    Vector2 seek = Vector2Normalize(target.position - enemy.position);

    Vector2 separation{};
    for (Slot<Enemy>& other_slot : state.enemies.data) {
        if (!other_slot.alive) continue;
        const Enemy& other_enemy = other_slot.ref;

        const Vector2 direction_from_other = enemy.position - other_enemy.position;
        float distance = Vector2Length(direction_from_other);

        const float personal_space_center_to_center = enemy.size + other_enemy.size + PERSONAL_SPACE;
        if (distance < 0.001f || distance > personal_space_center_to_center) continue;

        float strength = 1.0f - (distance / personal_space_center_to_center);

        separation += Vector2Normalize(direction_from_other) * strength;
    }

    for (Slot<Spawner>& other_slot : state.spawners.data) {
        if (!other_slot.alive) continue;
        const Spawner& spawner = other_slot.ref;

        const Vector2 direction_from_other = enemy.position - spawner.position;
        float distance = Vector2Length(direction_from_other);

        const float personal_space_center_to_center = enemy.size + SPAWNER_SIZE / 2 + PERSONAL_SPACE;
        if (distance < 0.001f || distance > personal_space_center_to_center) continue;

        float strength = 1.0f - (distance / personal_space_center_to_center);

        separation += Vector2Normalize(direction_from_other) * strength;
    }

    return Vector2Normalize(seek + separation);
}

Vector2 get_simple_follow_direction(Enemy& enemy, Targetable& target, GameState& state) {
    return Vector2Normalize(target.position - enemy.position);
}

void attack_melee(Enemy& enemy, Targetable& target, GameState& state) {
    // Out of range -> keep moving
    if (enemy.time_since_last_attack < enemy.attack_cooldown) return;

    apply_damage(state, target, enemy.damage);
    enemy.time_since_last_attack = 0;
}

void attack_ranged(Enemy& enemy, Targetable& target, GameState& state) {
    if (enemy.time_since_last_attack < enemy.attack_cooldown) return;

    CreateEntity(state.projectiles, Projectile{.type = ProjectileType::Enemy,
                                               .direction = Vector2Normalize(target.position - enemy.position),
                                               .position = enemy.position,
                                               .life_time = 2.0,
                                               .speed = 800,
                                               .damage = enemy.damage,
                                               .flags = TARGET_PLAYER | TARGET_TOWER});
    enemy.time_since_last_attack = 0;
}

using SeekBehaviorFn = Vector2 (*)(Enemy&, Targetable&, GameState&);
constexpr std::array<SeekBehaviorFn, static_cast<size_t>(SeekBehavior::Count)> seek_behavior_table = {
    get_simple_follow_direction, get_separation_direction};

using AttackBehaviorFn = void (*)(Enemy&, Targetable&, GameState& state);
constexpr std::array<AttackBehaviorFn, static_cast<size_t>(AttackBehavior::Count)> attack_behavior_table = {
    nullptr, attack_melee, attack_ranged};

Vector2 get_wander_direction(Enemy& enemy, const Vector2 center, const float range, const float min_idle,
                             const float max_idle) {
    // Move to target position
    if (Vector2Distance(enemy.position, enemy.target_position) > 2.5) {
        return Vector2Normalize(enemy.target_position - enemy.position);
    }

    const float delta_time = GetFrameTime();

    // Idle until enough time has passed
    if (enemy.remaining_idle_time > 0) {
        enemy.remaining_idle_time -= delta_time;
        return {};
    };

    // Set new wander target
    const float random_x = (float)GetRandomValue(0, 100) * (range / 100.0) - (range / 2.0);
    const float random_y = (float)GetRandomValue(0, 100) * (range / 100.0) - (range / 2.0);
    const Vector2 wander_offset = Vector2{.x = random_x, .y = random_y};

    enemy.target_position = center + wander_offset;
    enemy.remaining_idle_time = (float)GetRandomValue(0, 100) * (max_idle / 100.0) + min_idle;

    return Vector2Normalize(enemy.target_position - enemy.position);
}

const float WANDER_AROUND_RALLY = 100.0;
void UpdateEnemies(GameState& state) {
    for (Slot<Enemy>& slot : state.enemies.data) {
        if (!slot.alive) continue;

        Enemy& enemy = slot.ref;

        std::optional<Targetable> target =
            find_closest_target(enemy.position, state.targetables, TARGET_TOWER | TARGET_PLAYER);

        const float delta_time = GetFrameTime();

        enemy.time_since_last_attack += delta_time;
        Vector2 velocity = {};

        switch (enemy.state) {
            case EnemyState::Wander: {
                Spawner* home = GetEntity(state.spawners, enemy.home);

                if (Vector2Distance(enemy.position, target->position) < enemy.aggro_range) {
                    enemy.state = EnemyState::Seek;
                    if (home != nullptr) home->state = SpawnerState::UnderAttack;
                    break;
                }

                Vector2 wander_center = enemy.position;

                // TODO: If no home -> attempt a seek every x seconds
                if (home != nullptr) wander_center = home->position;

                velocity = get_wander_direction(enemy, wander_center, WANDER_RANGE, MIN_IDLE_TIME, MAX_IDLE_TIME) *
                           (enemy.speed * WANDER_SPEED_MODIFIER);

                break;
            }
            case EnemyState::Rally: {
                Spawner* home = GetEntity(state.spawners, enemy.home);
                if (home == nullptr) {
                    enemy.state = EnemyState::Seek;
                    break;
                }

                if (Vector2Distance(enemy.position, target->position) < enemy.aggro_range) {
                    enemy.state = EnemyState::Seek;
                    if (home != nullptr) home->state = SpawnerState::UnderAttack;
                    break;
                }

                if (!enemy.rallied && Vector2Distance(enemy.position, home->rally_position) < WANDER_AROUND_RALLY) {
                    enemy.rallied = true;
                    enemy.target_position = enemy.position;
                    // Enforce immediate wander when they arrive
                    enemy.remaining_idle_time = 0;
                }

                if (enemy.rallied) {
                    velocity = get_wander_direction(enemy, home->rally_position, WANDER_AROUND_RALLY,
                                                    MIN_RALLY_IDLE_TIME, MAX_RALLY_IDLE_TIME);
                } else {
                    velocity = Vector2Normalize(home->rally_position - enemy.position);
                }

                velocity *= enemy.speed;
                break;
            }
            case EnemyState::Seek: {
                if (!target.has_value()) {
                    enemy.state = EnemyState::Wander;
                    break;
                }

                enemy.target = target.value();

                velocity = seek_behavior_table[static_cast<size_t>(enemy.seek_behavior)](enemy, target.value(), state) *
                           enemy.speed;

                // -5 so the enemy does not immediately flip back to seek next frame
                if (Vector2Distance(enemy.target->position, enemy.position) < enemy.size * 2 + enemy.range - 5) {
                    enemy.state = EnemyState::Attack;
                }
                break;
            }
            case EnemyState::Attack: {
                if (!target.has_value()) {
                    enemy.state = EnemyState::Wander;
                    break;
                }

                enemy.target = target.value();

                if (Vector2Distance(enemy.target->position, enemy.position) > enemy.size * 2 + enemy.range) {
                    enemy.state = EnemyState::Seek;
                    break;
                }

                AttackBehaviorFn attack_behavior = attack_behavior_table[static_cast<size_t>(enemy.attack_behavior)];
                if (attack_behavior != nullptr) attack_behavior(enemy, enemy.target.value(), state);
                break;
            }
            default:
                break;
        }

        enemy.position += velocity * delta_time;
    }
}

void DrawEnemies(const EntityPool<Enemy>& enemies) {
    for (const Slot<Enemy>& enemy : enemies.data) {
        if (!enemy.alive) continue;

        DrawTextureEx(get_sprite("enemy"), enemy.ref.position, 0, (enemy.ref.size / 25) * 2, enemy.ref.color);
        DrawHealth(enemy.ref.position - Vector2{.x = 0, .y = enemy.ref.size + 20}, enemy.ref.health);

        std::string state_text;
        switch (enemy.ref.state) {
            case EnemyState::Attack:
                state_text = "Attack";
                break;
            case EnemyState::Rally:
                state_text = "Rally";
                break;
            case EnemyState::Seek:
                state_text = "Seek";
                break;
            default:
                state_text = "Wander";
                break;
        }
        DrawText(state_text.c_str(), enemy.ref.position.x, enemy.ref.position.y, 12, BLACK);
    }
}
