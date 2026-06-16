#pragma once

#include "core/asset_manager.hpp"
#include "core/random.hpp"
#include "raylib.h"

inline void play_sound(std::string sound_name, float random_pitch = 0.05f) {
    Sound sound = get_sound(sound_name);

    const float pitch_offset = random_float(-random_pitch, random_pitch);

    SetSoundPitch(sound, 1 + pitch_offset);
    PlaySound(sound);
}
