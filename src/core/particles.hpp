#pragma once

#include "core/data.hpp"
#include "core/entity_pool.hpp"
#include "raylib.h"

#include <algorithm>

// Emitter
//  - position
//  - type
//  - rate (particles per second)
//  - lifetime
//
// Particle
//  - position
//  - velocity
//  - size
//  - color
//  - age
//  - lifetime
//
// Emitter creates particles
// Particles come from a pool

struct ParticleTemplate {
    ParticleTemplate(Vec2F velocity, float size, Color color, float lifetime)
        : velocity(velocity), size(size), color(color), lifetime(lifetime) {}

    Vec2F velocity;
    float size;
    Color color;
    float lifetime;
};

struct Particle {
    Particle(Vec2F position, Vec2F velocity, float start_size, Color color, float lifetime)
        : position(position), velocity(velocity), start_size(start_size), size(start_size), color(color),
          lifetime(lifetime) {}

    Vec2F position;
    Vec2F velocity;

    float start_size;
    float size;

    Color color;

    float lifetime;
    float age = 0.0f;

    void update(const float deltaTime) {
        age += deltaTime;

        const float t = std::ranges::clamp(age / lifetime, 0.0f, 1.0f);

        const unsigned char alpha = (1.0f - t) * 255;
        position += velocity * deltaTime;
        color.a = alpha;
        size = (1.0f - t) * start_size;
    }
};

enum class EmitterType {
    Point
};

struct Emitter {
    EmitterType type = EmitterType::Point;
    Vec2F position;
    ParticleTemplate particle_template;
    float rate;
    float duration;
    float age = 0;

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
        CreateEntity(pool, Particle(position, particle_template.velocity, particle_template.size,
                                    particle_template.color, particle_template.lifetime));
    }
};

struct ParticleSystem {
    std::vector<Emitter> active_emitters;
    EntityPool<Particle> particle_pool;

    void play(Emitter emitter) { active_emitters.push_back(emitter); }

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
};
