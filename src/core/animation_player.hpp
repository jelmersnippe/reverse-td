#pragma once

#include "core/data.hpp"
#include "core/renderer.hpp"
#include <cassert>
#include <string>
#include <utility>

struct AnimationInfo {
    std::string sprite_name;
    Vec2 sprite_size;
    int frame_count;
    float time_per_frame = 0.2f;
};

struct AnimationPlayer {
    bool playing = false;
    int begin_frame = 0;
    int current_frame = 0;
    float time_since_last_frame = 0;

    AnimationInfo info;

    AnimationPlayer(AnimationInfo info) : info(std::move(info)) {}

    void play(int begin_frame = 0) {
        if (this->playing) return;

        assert(begin_frame < info.frame_count && "Can't select a begin frame that's higher than the frame count");
        this->begin_frame = begin_frame;
        this->current_frame = begin_frame;

        this->playing = true;
    }

    void stop() {
        if (!this->playing) return;

        this->current_frame = this->begin_frame;

        this->playing = false;
    }

    void update(float delta_time) {
        if (!playing) return;

        time_since_last_frame += delta_time;

        while (time_since_last_frame >= info.time_per_frame) {
            current_frame = (current_frame + 1) % info.frame_count;
            time_since_last_frame -= info.time_per_frame;
        }
    }

    void draw(Vec2F center, Vec2F size, bool flip_x = false) const {
        render_sprite(
            SpriteInfo(this->info.sprite_name, this->info.sprite_size, this->current_frame, {.x = flip_x, .y = false}),
            center, size);
    }
};
