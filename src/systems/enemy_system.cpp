#include "enemy_system.hpp"

#include "array"
#include "core/entity_pool.hpp"
#include "entities/enemy.hpp"
#include "game_state.hpp"
#include "raylib.h"
#include "raymath.h"
#include "systems/targeting.hpp"

Targetable get_target(Enemy& enemy, GameState& state) {
    Targetable target = find_closest_target(enemy.position, build_targetables(state), TARGET_TOWER | TARGET_PLAYER);

    Vector2 target_position = enemy.position;

    switch (target.flags) {
        case TARGET_PLAYER: {
            target_position = state.player.position;
            break;
        }
        case TARGET_TOWER: {
            Tower* tower = GetEntity(state.towers, target.handle);
            if (tower != nullptr) target_position = tower->position;
            break;
        }
        default:
            break;
    }

    return target;
}

Vector2 get_separation_velocity(Enemy& enemy, Targetable& target, GameState& state) {
    Vector2 seek = Vector2Normalize(target.position - enemy.position);

    Vector2 separation{};
    for (Slot<Enemy>& other_slot : state.enemies.data) {
        if (!other_slot.alive) continue;
        const Enemy& other_enemy = other_slot.ref;

        const Vector2 direction_from_other = enemy.position - other_enemy.position;
        float distance = Vector2Length(direction_from_other);

        if (distance < 0.001f || distance > (enemy.size + PERSONAL_SPACE)) continue;

        float strength = 1.0f - (distance / (enemy.size + PERSONAL_SPACE));

        separation += Vector2Normalize(direction_from_other) * strength;
    }

    return Vector2Normalize(seek + separation) * enemy.speed;
}

Vector2 get_simple_follow_velocity(Enemy& enemy, Targetable& target, GameState& state) {
    return Vector2Normalize(target.position - enemy.position) * enemy.speed;
}

// TODO: Switch this to state machine. Bool now says if has attacked = don't move
bool attack_melee(Enemy& enemy, Targetable& target, GameState& state) {
    // Out of range -> keep moving
    if (Vector2Distance(target.position, enemy.position) > enemy.size * 2 + enemy.range) return false;

    // Cooldown -> Don't move but also don't attack
    if (enemy.time_since_last_attack < enemy.attack_cooldown) return true;

    apply_damage(state, target, enemy.damage);
    enemy.time_since_last_attack = 0;

    return true;
}

bool attack_ranged(Enemy& enemy, Targetable& target, GameState& state) {
    // Out of range -> keep moving
    if (Vector2Distance(target.position, enemy.position) > enemy.size * 2 + enemy.range) return false;

    // Cooldown -> Don't move but also don't attack
    if (enemy.time_since_last_attack < enemy.attack_cooldown) return true;

    CreateEntity(state.projectiles, Projectile{.velocity = Vector2Normalize(target.position - enemy.position) * 600,
                                               .position = enemy.position,
                                               .life_time = 2.0,
                                               .damage = enemy.damage,
                                               .flags = TARGET_PLAYER | TARGET_TOWER});
    enemy.time_since_last_attack = 0;

    return true;
}

using SeekBehaviorFn = Vector2 (*)(Enemy&, Targetable&, GameState&);
constexpr std::array<SeekBehaviorFn, static_cast<size_t>(SeekBehavior::Count)> seek_behavior_table = {
    get_simple_follow_velocity, get_separation_velocity};

using AttackBehaviorFn = bool (*)(Enemy&, Targetable&, GameState& state);
constexpr std::array<AttackBehaviorFn, static_cast<size_t>(AttackBehavior::Count)> attack_behavior_table = {
    nullptr, attack_melee, attack_ranged};

void UpdateEnemies(GameState& state) {
    for (Slot<Enemy>& slot : state.enemies.data) {
        if (!slot.alive) continue;

        Enemy& enemy = slot.ref;

        const float delta_time = GetFrameTime();

        enemy.time_since_last_attack += delta_time;

        Targetable target = get_target(enemy, state);

        auto attack_behavior = attack_behavior_table[static_cast<size_t>(enemy.attack_behavior)];

        if (attack_behavior != nullptr && attack_behavior(enemy, target, state)) continue;

        const Vector2 velocity = seek_behavior_table[static_cast<size_t>(enemy.seek_behavior)](enemy, target, state);

        enemy.position += velocity * delta_time;
    }
}
