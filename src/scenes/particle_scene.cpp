#include "scenes/particle_scene.hpp"

#include "core/data.hpp"
#include "core/particles.hpp"
#include "raylib.h"

namespace {

ParticleSystem particle_system;

const ParticleTemplate PARTICLE_TEMPLATE = ParticleTemplate(Vec2F{.x = 0, .y = -100}, 10, RED, 1.5f);

void Update(GameState& state) {
    const Vector2 mouse_pos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        particle_system.play({.position = {.x = mouse_pos.x, .y = mouse_pos.y},
                              .particle_template = PARTICLE_TEMPLATE,
                              .rate = 4,
                              .duration = 5});
    }

    particle_system.update(GetFrameTime());
}

void Draw(GameState& state) {
    ClearBackground(GRAY);

    for (const Slot<Particle>& slot : particle_system.particle_pool.data) {
        if (!slot.alive) continue;

        const Particle& particle = slot.ref;

        const Vec2F top_left = particle.position - Vec2F{.x = particle.size / 2, .y = particle.size / 2};

        DrawRectangle((int)top_left.x, (int)top_left.y, (int)particle.size, (int)particle.size, particle.color);
    }
}
} // namespace

const Scene PARTICLE_SCENE = {.name = "Particles", .init = nullptr, .update = Update, .draw = Draw, .destroy = nullptr};
