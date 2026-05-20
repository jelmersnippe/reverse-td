#include "enemy_system.hpp"

#include "array"
#include "core/entity_pool.hpp"
#include "entities/enemy.hpp"
#include "game_state.hpp"
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

    CreateEntity(state.projectiles, Projectile{.velocity = Vector2Normalize(target.position - enemy.position) * 600,
                                               .position = enemy.position,
                                               .life_time = 2.0,
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

void UpdateEnemies(GameState& state) {
    for (Slot<Enemy>& slot : state.enemies.data) {
        if (!slot.alive) continue;

        Enemy& enemy = slot.ref;

        const float delta_time = GetFrameTime();

        enemy.time_since_last_attack += delta_time;
        Vector2 velocity = {};

        switch (enemy.state) {
            case EnemyState::Rally: {
                throw "Not implemented";
                break;
            }
            case EnemyState::Wander: {
                if (Vector2Distance(enemy.position, enemy.target_position) < 1.0) {
                    if (enemy.remaining_idle_time > 0) {
                        enemy.remaining_idle_time -= delta_time;
                        break;
                    };

                    const float random_x =
                        (float)GetRandomValue(0, 100) * (WANDER_RANGE / 100.0) - (WANDER_RANGE / 2.0);
                    const float random_y =
                        (float)GetRandomValue(0, 100) * (WANDER_RANGE / 100.0) - (WANDER_RANGE / 2.0);
                    const Vector2 wander_offset = Vector2{.x = random_x, .y = random_y};

                    Spawner* home = GetEntity(state.spawners, enemy.home);
                    if (home == nullptr) {
                        // We could make the enemy turn into seek/go into a frenzy because it's home was destroyed
                        // If there is no targets (like in a test scene) this will perma flip state though
                        // enemy.state = EnemyState::Seek;
                        // break;
                        enemy.target_position += wander_offset;
                    } else {
                        enemy.target_position = home->position + wander_offset;
                    }

                    enemy.remaining_idle_time = (float)GetRandomValue(0, 100) * (MAX_IDLE_TIME / 100.0) + MIN_IDLE_TIME;
                }

                velocity =
                    Vector2Normalize(enemy.target_position - enemy.position) * (enemy.speed * WANDER_SPEED_MODIFIER);
                break;
            }
            case EnemyState::Seek: {
                std::optional<Targetable> target =
                    find_closest_target(enemy.position, build_targetables(state), TARGET_TOWER | TARGET_PLAYER);

                if (!target.has_value()) {
                    // If the enemy has no home and can't find a target it will perma flip between states
                    enemy.state = EnemyState::Wander;
                    break;
                }

                enemy.target = target.value();

                velocity = seek_behavior_table[static_cast<size_t>(enemy.seek_behavior)](enemy, target.value(), state) *
                           enemy.speed;
                break;
            }
            case EnemyState::Attack: {
                if (!enemy.target.has_value() ||
                    Vector2Distance(enemy.target->position, enemy.position) > enemy.size * 2 + enemy.range) {
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

        DrawCircle(enemy.ref.position.x, enemy.ref.position.y, enemy.ref.size, enemy.ref.color);
        DrawHealth(enemy.ref.position - Vector2{.x = 0, .y = enemy.ref.size + 20}, enemy.ref.health);
    }
}
