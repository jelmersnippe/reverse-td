#include "player_system.hpp"

#include "blueprints/particles.hpp"
#include "core/camera.hpp"
#include "core/input.hpp"
#include "core/renderer.hpp"
#include "core/sound.hpp"
#include "game_state.hpp"
#include "globals.hpp"

void Update(Player& player, GameState& state) {
    const float delta_time = GetFrameTime();

    if (input_frame.is_key_down(Key::W)) player.direction += {.x = 0, .y = -1};
    if (input_frame.is_key_down(Key::S)) player.direction += {.x = 0, .y = 1};
    if (input_frame.is_key_down(Key::A)) player.direction += {.x = -1, .y = 0};
    if (input_frame.is_key_down(Key::D)) player.direction += {.x = 1, .y = 0};

    if (player.direction.x == 0 && player.direction.y == 0) {
        player.animation_player.stop();
    } else {
        player.animation_player.play();
    }
    player.animation_player.update(delta_time);

    player.weapon.time_since_last_shot += delta_time;
    player.time_since_last_damage_taken += delta_time;

    const bool can_regen = player.weapon.time_since_last_shot >= player.out_of_combat_time &&
                           player.time_since_last_damage_taken >= player.out_of_combat_time &&
                           player.health.current < player.health.max;

    if (!can_regen) {
        player.regenerating = false;
    } else {
        if (!player.regenerating) {
            player.regenerating = true;
            player.time_since_last_regen = 0;
        }

        player.time_since_last_regen += delta_time;

        if (player.time_since_last_regen >= player.regen_time) {
            player.health.current++;

            player.time_since_last_regen -= player.regen_time;
        }
    }

    float speed = player.speed;
    if (player.weapon.time_since_last_shot < TIME_BETWEEN_SHOTS) { speed *= player.attacking_speed_modifier; }

    Vec2F velocity = player.direction.normalized() * speed * delta_time;

    Vec2F new_position = player.position;

    new_position.x += velocity.x;
    const CollisionResult collision_x = check_player_collision(state, new_position);

    new_position = player.position;
    new_position.y += velocity.y;
    const CollisionResult collision_y = check_player_collision(state, new_position);

    if (!collision_x.collided) player.position.x += velocity.x;
    if (!collision_y.collided) player.position.y += velocity.y;

    player.direction = {.x = 0, .y = 0};

    const Vec2F mouse_position = get_mouse_world_position(state.camera);
    float mouse_angle = player.position.angle_to(mouse_position);
    bool flipped = mouse_angle > 90 || mouse_angle < -90;

    // TODO: Should show a death state / restart the game
    if (player.health.current <= 0) { state.should_exit = true; }

    if (player.weapon.time_since_last_shot < player.weapon.recovery_time) {
        player.weapon.current_knockback =
            (player.weapon.recoil * (1 - (player.weapon.time_since_last_shot / player.weapon.recovery_time)));
    } else {
        player.weapon.current_knockback = {.x = 0, .y = 0};
    }

    Vec2F hand_offset = player.weapon.offset + player.weapon.current_knockback;
    if (flipped) hand_offset.y = -hand_offset.y;

    Vec2F rotated_hand_offset = hand_offset.rotate(mouse_angle);
    player.weapon.position = player.position + rotated_hand_offset;
    player.weapon.angle = player.weapon.position.angle_to(mouse_position);

    if (input_frame.is_mouse_down(Mouse::Left) &&
        player.weapon.time_since_last_shot >= player.weapon.time_between_shots) {

        Vec2F projectile_spawn_point = player.weapon.projectile_spawn_point;
        if (flipped) projectile_spawn_point.y = -projectile_spawn_point.y;

        const Vec2F barrel_end_pos = player.weapon.position + projectile_spawn_point.rotate(mouse_angle);

        const float spread_angle = random_float(-player.weapon.spread, player.weapon.spread);
        const Vec2F direction = barrel_end_pos.direction_to(mouse_position).normalized().rotate(spread_angle);

        CreateEntity(state.projectiles, Projectile{.direction = direction,
                                                   .position = barrel_end_pos,
                                                   .life_time = 1.0,
                                                   .damage = player.weapon.damage,
                                                   .flags = TARGET_SPAWNER | TARGET_ENEMY});
        player.weapon.time_since_last_shot = 0;

        MUZZLE_FLASH_PARTICLE.rotation = direction.angle();
        MUZZLE_FLASH_PARTICLE.display.sprite_info.frame = random_int(0, 3);
        MUZZLE_FLASH_EMITTER.particle_template = MUZZLE_FLASH_PARTICLE;
        MUZZLE_FLASH_EMITTER.position = barrel_end_pos;
        player.weapon.particles.play(MUZZLE_FLASH_EMITTER);

        MUZZLE_SMOKE_EMITTER.position = barrel_end_pos;
        MUZZLE_SMOKE_EMITTER.direction = direction;
        player.weapon.particles.play(MUZZLE_SMOKE_EMITTER);

        play_sound("pistol");

        player.weapon.current_knockback = player.weapon.recoil;
    }

    player.weapon.particles.update(delta_time);
}

void UpdatePlayers(GameState& state) {
    for (Slot<Player>& player : state.players.data) {
        if (!player.alive) continue;

        Update(player.ref, state);
    }
}

void DrawPlayers(const EntityPool<Player>& players, const Camera2D& camera) {
    for (const Slot<Player>& player : players.data) {
        if (!player.alive) continue;

        const Vec2F mouse_position = get_mouse_world_position(camera);
        Vec2F direction = player.ref.position.direction_to(mouse_position);
        bool flipped = direction.x < 0;

        player.ref.weapon.particles.draw();

        // Player
        player.ref.animation_player.draw(Vec2F{.x = player.ref.position.x, .y = player.ref.position.y},
                                         {.x = PLAYER_SIZE, .y = PLAYER_SIZE}, flipped);

        // Gun
        SpriteInfo info = player.ref.weapon.sprite;
        Vec2F render_size = {.x = info.size.x * 2.0f, .y = info.size.y * 2.0f};
        info.should_flip.y = flipped;
        render_sprite(info, player.ref.weapon.position, render_size, player.ref.weapon.angle, WHITE);

        // Health
        const Vec2F health_position = player.ref.position - Vec2F{.x = 0, .y = PLAYER_SIZE};
        DrawHealth(health_position, player.ref.health);

        if (player.ref.regenerating) {
            render_text("Regenerating", health_position - Vec2F{.x = 0, .y = -5}, 10, BLACK);
        }
    }
}
