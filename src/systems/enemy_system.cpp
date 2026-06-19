#include "enemy_system.hpp"

#include "array"
#include "core/asset_manager.hpp"
#include "core/entity_pool.hpp"
#include "core/random.hpp"
#include "core/renderer.hpp"
#include "entities/enemy.hpp"
#include "entities/projectile.hpp"
#include "entities/spawner.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raylib.h"
#include "systems/targeting.hpp"
#include <optional>

Vec2F get_separation_direction(Enemy& enemy, Targetable& target, GameState& state) {
    Vec2F seek = enemy.position.direction_to(target.position).normalized();

    Vec2F separation{};
    for (Slot<Enemy>& other_slot : state.enemies.data) {
        if (!other_slot.alive) continue;
        const Enemy& other_enemy = other_slot.ref;

        const Vec2F direction_from_other = other_enemy.position.direction_to(enemy.position);
        float distance = direction_from_other.length();

        const float personal_space_center_to_center = enemy.size + other_enemy.size + PERSONAL_SPACE;
        if (distance < 0.001f || distance > personal_space_center_to_center) continue;

        float strength = 1.0f - (distance / personal_space_center_to_center);

        separation += direction_from_other.normalized() * strength;
    }

    for (Slot<Spawner>& other_slot : state.spawners.data) {
        if (!other_slot.alive) continue;
        const Spawner& spawner = other_slot.ref;

        const Vec2F direction_from_other = spawner.position.direction_to(enemy.position);
        float distance = direction_from_other.length();

        const float personal_space_center_to_center = enemy.size + SPAWNER_SIZE / 2 + PERSONAL_SPACE;
        if (distance < 0.001f || distance > personal_space_center_to_center) continue;

        float strength = 1.0f - (distance / personal_space_center_to_center);

        separation += direction_from_other.normalized() * strength;
    }

    return (seek + separation).normalized();
}

Vec2F get_simple_follow_direction(Enemy& enemy, Targetable& target, GameState&) {
    return enemy.position.direction_to(target.position).normalized();
}

void attack_melee(Enemy& enemy, Targetable& target, GameState& state) {
    // Out of range -> keep moving
    if (enemy.time_since_last_attack < enemy.attack_cooldown) return;

    apply_damage(state, target, enemy.damage, enemy.position.direction_to(target.position));
    enemy.time_since_last_attack = 0;
}

void attack_ranged(Enemy& enemy, Targetable& target, GameState& state) {
    if (enemy.time_since_last_attack < enemy.attack_cooldown) return;

    CreateEntity(state.projectiles, Projectile{.type = ProjectileType::Enemy,
                                               .direction = enemy.position.direction_to(target.position).normalized(),
                                               .position = enemy.position,
                                               .life_time = 2.0,
                                               .speed = 400,
                                               .damage = enemy.damage,
                                               .flags = TARGET_PLAYER | TARGET_TOWER});
    enemy.time_since_last_attack = 0;
}

using SeekBehaviorFn = Vec2F (*)(Enemy&, Targetable&, GameState&);
constexpr std::array<SeekBehaviorFn, static_cast<size_t>(SeekBehavior::Count)> seek_behavior_table = {
    get_simple_follow_direction, get_separation_direction};

using AttackBehaviorFn = void (*)(Enemy&, Targetable&, GameState& state);
constexpr std::array<AttackBehaviorFn, static_cast<size_t>(AttackBehavior::Count)> attack_behavior_table = {
    nullptr, attack_melee, attack_ranged};

Vec2F get_wander_direction(Enemy& enemy, const Vec2F center, const float range, const float min_idle,
                           const float max_idle) {
    // Move to target position
    if (enemy.position.distance_to(enemy.target_position) > 2.5) {
        return enemy.position.direction_to(enemy.target_position).normalized();
    }

    const float delta_time = GetFrameTime();

    // Idle until enough time has passed
    if (enemy.remaining_idle_time > 0) {
        enemy.remaining_idle_time -= delta_time;
        return {};
    };

    // Set new wander target
    const float random_x = random_float(-range, range);
    const float random_y = random_float(-range, range);
    const Vec2F wander_offset = Vec2F{.x = random_x, .y = random_y};

    enemy.target_position = center + wander_offset;
    enemy.remaining_idle_time = random_float(min_idle, max_idle);

    return enemy.position.direction_to(enemy.target_position).normalized();
}

void UpdateEnemies(GameState& state) {
    for (Slot<Enemy>& slot : state.enemies.data) {
        if (!slot.alive) continue;
        Enemy& enemy = slot.ref;

        const float delta_time = GetFrameTime();

        if (enemy.dead) {
            enemy.time_dead += delta_time;
            if (enemy.time_dead > 1.5f) {
                DestroyEntity(state.enemies, slot.handle);
                return;
            }
        };

        if (enemy.hit_flash_remaining > 0.0f) enemy.hit_flash_remaining -= delta_time;

        enemy.health.update();

        Vec2F velocity = {};

        if (enemy.knockback.active) {
            enemy.knockback.time_active += delta_time;
            // Knockback strength is less for larger enemies
            const float size_modifier = 32.0f / enemy.size;
            const float knockback_strength = enemy.knockback.strength * size_modifier * size_modifier;
            const Vec2F knockback = enemy.knockback.direction * knockback_strength;

            if (enemy.knockback.decays_over_time) {
                velocity = knockback * (1 - (enemy.knockback.time_active / enemy.knockback.recovery_time));
            } else {
                velocity = knockback;
            }

            if (enemy.knockback.time_active >= enemy.knockback.recovery_time) enemy.knockback.active = false;
        } else if (!enemy.dead) {
            std::optional<Targetable> target =
                find_closest_target(enemy.position, state.targetables, TARGET_TOWER | TARGET_PLAYER);

            enemy.time_since_last_attack += delta_time;

            if (!target.has_value()) {
                enemy.state = EnemyState::Wander;
                break;
            }

            switch (enemy.state) {
                case EnemyState::Wander: {
                    Spawner* home = GetEntity(state.spawners, enemy.home);

                    if (target.has_value() && enemy.position.distance_to(target->position) < enemy.aggro_range) {
                        enemy.state = EnemyState::Seek;
                        if (home != nullptr) home->state = SpawnerState::UnderAttack;
                        break;
                    }

                    Vec2F wander_center = enemy.position;

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

                    if (enemy.position.distance_to(target->position) < enemy.aggro_range) {
                        enemy.state = EnemyState::Seek;
                        if (home != nullptr) home->state = SpawnerState::UnderAttack;
                        break;
                    }

                    if (!enemy.rallied && enemy.position.distance_to(home->rally_position) < WANDER_AROUND_RALLY) {
                        enemy.rallied = true;
                        enemy.target_position = enemy.position;
                        // Enforce immediate wander when they arrive
                        enemy.remaining_idle_time = 0;
                    }

                    if (enemy.rallied) {
                        velocity = get_wander_direction(enemy, home->rally_position, WANDER_AROUND_RALLY,
                                                        MIN_RALLY_IDLE_TIME, MAX_RALLY_IDLE_TIME);
                    } else {
                        velocity = enemy.position.direction_to(home->rally_position).normalized();
                    }

                    velocity *= enemy.speed;
                    break;
                }
                case EnemyState::Seek: {
                    enemy.target = target.value();

                    velocity =
                        seek_behavior_table[static_cast<size_t>(enemy.seek_behavior)](enemy, target.value(), state) *
                        enemy.speed;

                    // -5 so the enemy does not immediately flip back to seek next frame
                    if (enemy.target->position.distance_to(enemy.position) < enemy.size * 2 + enemy.range - 5) {
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

                    if (enemy.target->position.distance_to(enemy.position) > enemy.size * 2 + enemy.range) {
                        enemy.state = EnemyState::Seek;
                        break;
                    }

                    AttackBehaviorFn attack_behavior =
                        attack_behavior_table[static_cast<size_t>(enemy.attack_behavior)];
                    if (attack_behavior != nullptr) attack_behavior(enemy, enemy.target.value(), state);
                    break;
                }
                default:
                    break;
            }
        }

        enemy.position += velocity * delta_time;

        enemy.particles.update(delta_time);
    }
}

void draw_debug_info(const Enemy& enemy) {
    std::string state_text;
    switch (enemy.state) {
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
    render_text(state_text, enemy.position, 12, BLACK);
}

void DrawEnemies(const GameState& state) {
    for (const Slot<Enemy>& enemy : state.enemies.data) {
        if (!enemy.alive) continue;
        Shader flash_shader = get_shader("flash");

        if (enemy.ref.hit_flash_remaining > 0) {
            float flash_color[3] = {1.0f, 1.0f, 1.0f};
            float flash_volume = 1.0f;

            int volume_loc = GetShaderLocation(flash_shader, "volume");
            int color_loc = GetShaderLocation(flash_shader, "color");

            SetShaderValue(flash_shader, volume_loc, &flash_volume, SHADER_UNIFORM_FLOAT);
            SetShaderValue(flash_shader, color_loc, flash_color, SHADER_UNIFORM_VEC3);
            BeginShaderMode(flash_shader);
        }

        Color color = enemy.ref.color;
        if (enemy.ref.dead) color.a = (255 * 0.4f);

        const Vec2F scale =
            Vec2F{.x = enemy.ref.size, .y = enemy.ref.size} / Vec2F{.x = DEFAULT_SPRITE_SIZE, .y = DEFAULT_SPRITE_SIZE};
        render_sprite(SpriteInfo("enemy", {.x = DEFAULT_SPRITE_SIZE, .y = DEFAULT_SPRITE_SIZE}, scale),
                      enemy.ref.position, 0, color);

        if (enemy.ref.hit_flash_remaining > 0) EndShaderMode();

        if (!enemy.ref.dead) {
            DrawHealth(enemy.ref.position - Vec2F{.x = 0, .y = enemy.ref.size + 20}, enemy.ref.health);
            draw_debug_info(enemy.ref);
        }

        enemy.ref.particles.draw();
    }
}
