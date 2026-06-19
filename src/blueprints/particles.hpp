#pragma once

#include "core/particles.hpp"
#include "globals.hpp"

inline ParticleTemplate HIT_PARTICLE = ParticleTemplate({
    .speed = {.start = {.min = 80, .max = 120}, .end = {.min = 10, .max = 20}},
    .size = {.start = {.min = 4, .max = 8}, .end = {.min = 0, .max = 2}},
    .color = {.start = Color{.r = 230, .g = 41, .b = 55, .a = 255}, .end = Color{.r = 230, .g = 41, .b = 55, .a = 0}},
    .lifetime = {.min = 0.3f, .max = 0.6f},
});
inline Emitter HIT_EMITTER = Emitter{.position = {.x = 0, .y = 0},
                                     .direction = {.x = 0, .y = 0},
                                     .spread = 30,
                                     .particle_template = HIT_PARTICLE,
                                     .rate = 0,
                                     .duration = 0,
                                     .burst = 10};

inline Emitter DEATH_EMITTER = Emitter{.position = {.x = 0, .y = 0},
                                       .direction = {.x = 0, .y = 0},
                                       .spread = 360,
                                       .particle_template = HIT_PARTICLE,
                                       .rate = 0,
                                       .duration = 0,
                                       .burst = 30};

inline const ParticleTemplate MUZZLE_SMOKE_PARTICLE = ParticleTemplate({
    .speed = {.start = {.min = 60, .max = 80}, .end = {.min = 5, .max = 20}},
    .size = {.start = {.min = 2, .max = 4}, .end = {.min = 0, .max = 1}},
    .color = {.start = WHITE, .end = Color(255, 255, 255, 0)},
    .lifetime = {.min = 0.1f, .max = 0.3f},
});
inline Emitter MUZZLE_SMOKE_EMITTER = Emitter{.position = {.x = 0, .y = 0},
                                              .direction = {.x = 0, .y = 0},
                                              .spread = 40,
                                              .particle_template = MUZZLE_SMOKE_PARTICLE,
                                              .rate = 0,
                                              .duration = 0,
                                              .burst = 30};

inline ParticleTemplate MUZZLE_FLASH_PARTICLE = ParticleTemplate({
    .display = {.type = ParticleDisplayType::Sprite,
                .sprite_info = {"muzzle_flash",
                                {.x = DEFAULT_SPRITE_SIZE, .y = DEFAULT_SPRITE_SIZE},
                                {.x = 2, .y = 2}}},
    .speed = {.start = {.min = 0, .max = 0}, .end = {.min = 0, .max = 0}},
    .size = {.start = {.min = 32, .max = 32}, .end = {.min = 32, .max = 32}},
    .color = {.start = WHITE, .end = WHITE},
    .lifetime = {.min = 0.03f, .max = 0.08f},
});
inline Emitter MUZZLE_FLASH_EMITTER = Emitter{.position = {.x = 0, .y = 0},
                                              .direction = {.x = 0, .y = 0},
                                              .spread = 5,
                                              .particle_template = MUZZLE_FLASH_PARTICLE,
                                              .rate = 0,
                                              .duration = 0,
                                              .burst = 1};
