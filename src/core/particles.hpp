#pragma once

#include "core/data.hpp"
#include "core/entity_pool.hpp"
#include "core/random.hpp"
#include "core/renderer.hpp"
#include "raylib.h"

#include <algorithm>
#include <utility>

template <typename T> struct Range {
    T min;
    T max;

    T get_random() {
        if (min == max) return min;

        return random_float(min, max);
    }
};

template <typename T> struct ChangeOverLifetime {
    T start;
    T end;
};

enum class ParticleDisplayType {
    Sprite,
    Rectangle,
    Circle
};
struct ParticleDisplay {
    ParticleDisplayType type;
    SpriteInfo sprite_info = {};
};

struct ParticleTemplate {
    ParticleDisplay display = {.type = ParticleDisplayType::Rectangle};
    ChangeOverLifetime<Range<float>> speed;
    ChangeOverLifetime<Range<float>> size;
    ChangeOverLifetime<Color> color;
    Range<float> lifetime;
    float rotation = 0;
};

struct Particle {
    Particle(ParticleDisplay display, Vec2F position, Vec2F direction, float rotation, ChangeOverLifetime<float> speed,
             ChangeOverLifetime<float> size, ChangeOverLifetime<Color> color, float lifetime)
        : display(std::move(display)), position(position), velocity(direction * speed.start), rotation(rotation),
          speed_config(speed), speed(speed_config.start), size_config(size), size(size_config.start),
          color_config(color), color(color_config.start), lifetime(lifetime) {}

    ParticleDisplay display;

    Vec2F position;
    Vec2F velocity;
    float rotation;

    ChangeOverLifetime<float> speed_config;
    float speed;

    ChangeOverLifetime<float> size_config;
    float size;

    ChangeOverLifetime<Color> color_config;
    Color color;

    float lifetime;
    float age = 0.0f;

    void update(const float delta_time) {
        age += delta_time;

        const float t = std::ranges::clamp(age / lifetime, 0.0f, 1.0f);

        if (!ColorIsEqual(color_config.start, color_config.end)) {
            const unsigned char r = std::lerp(color_config.start.r, color_config.end.r, t);
            const unsigned char g = std::lerp(color_config.start.g, color_config.end.g, t);
            const unsigned char b = std::lerp(color_config.start.b, color_config.end.b, t);
            const unsigned char a = std::lerp(color_config.start.a, color_config.end.a, t);
            color = Color(r, g, b, a);
        }

        if (size_config.start != size_config.end) size = std::lerp(size_config.start, size_config.end, t);

        if (speed_config.start != speed_config.end) {
            speed = std::lerp(speed_config.start, speed_config.end, t);

            Vec2F dir = velocity.normalized();
            velocity = Vec2F{.x = dir.x, .y = dir.y} * speed;
        }

        position += velocity * delta_time;
    }
};

enum class EmitterType {
    Point,
    Circle,
    Box
};

struct Emitter {
    EmitterType type = EmitterType::Point;
    Vec2F position;
    Vec2F direction = {};
    float spread = 0;

    Vec2F box_size = {};
    float radius = 0;

    ParticleTemplate particle_template;
    float rate;
    float duration;
    float age = 0;

    int burst = 0;

    float time_since_last = 0;

    void update(const float deltaTime, EntityPool<Particle>& pool) {
        age += deltaTime;
        time_since_last += deltaTime;

        const float time_between_particles = 1.0f / rate;
        while (time_since_last > time_between_particles) {
            emit(pool);
            time_since_last -= time_between_particles;
        }
    }

    void emit(EntityPool<Particle>& pool) {
        ChangeOverLifetime<float> speed = {.start = particle_template.speed.start.get_random(),
                                           .end = particle_template.speed.end.get_random()};
        ChangeOverLifetime<float> size = {.start = particle_template.size.start.get_random(),
                                          .end = particle_template.size.end.get_random()};
        float lifetime = particle_template.lifetime.get_random();

        float base_angle = std::atan2(direction.y, direction.x);
        float spread_rad = spread * DEG2RAD;
        float angle_offset = random_float(-spread_rad, spread_rad);
        float angle = base_angle + angle_offset;

        Vec2F dir = {.x = std::cos(angle), .y = std::sin(angle)};

        Vec2F particle_pos = position;
        switch (type) {
            case EmitterType::Point:
                break;
            case EmitterType::Circle: {
                const float random_x = random_float(-radius, radius);
                const float random_y = random_float(-radius, radius);
                particle_pos += Vec2F{.x = random_x, .y = random_y};
                break;
            }
            case EmitterType::Box: {
                const float random_x = random_float(0, box_size.x);
                const float random_y = random_float(0, box_size.y);
                Vec2F position_in_box = {.x = random_x, .y = random_y};
                particle_pos += position_in_box;
                break;
            }
        }

        CreateEntity(pool, Particle(particle_template.display, particle_pos, dir, particle_template.rotation, speed,
                                    size, particle_template.color, lifetime));
    }
};

struct ParticleSystem {
    std::vector<Emitter> active_emitters;
    EntityPool<Particle> particle_pool;

    void play(Emitter emitter) {
        active_emitters.push_back(emitter);

        for (int i = 0; i < emitter.burst; i++) {
            emitter.emit(particle_pool);
        }
    }

    void update(const float deltaTime) {
        for (Emitter& emitter : active_emitters) {
            emitter.update(deltaTime, particle_pool);
        }

        std::erase_if(active_emitters, [](const Emitter& e) { return e.age > e.duration; });

        for (Slot<Particle>& slot : particle_pool.data) {
            if (!slot.alive) continue;

            slot.ref.update(deltaTime);

            if (slot.ref.age > slot.ref.lifetime) { DestroyEntity(particle_pool, slot.handle); }
        }
    }

    void draw() const {
        for (auto& slot : particle_pool.data) {
            if (!slot.alive) continue;

            const Particle& particle = slot.ref;

            switch (particle.display.type) {
                case ParticleDisplayType::Sprite: {
                    SpriteInfo info_to_render = particle.display.sprite_info;
                    Vec2F scale = Vec2F{.x = particle.size, .y = particle.size} / Vec2F::from_vec2(info_to_render.size);
                    info_to_render.scale = scale;

                    render_sprite(info_to_render, particle.position, particle.rotation, particle.color);
                    break;
                }
                case ParticleDisplayType::Rectangle:
                    render_rectangle(particle.position, {.x = particle.size, .y = particle.size}, particle.color);
                    break;
                case ParticleDisplayType::Circle:
                    render_circle(particle.position, particle.size, particle.color);
                    break;
            }
        }
    }
};
