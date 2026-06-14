#pragma once

#include "core/asset_manager.hpp"
#include "core/data.hpp"
#include "raylib.h"
#include <cassert>
#include <string>

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

    Texture2D sprite = {};
    AnimationInfo info;

    AnimationPlayer(AnimationInfo info) : info(info) {
        this->sprite = get_sprite(info.sprite_name);

        assert(this->sprite.width % this->info.sprite_size.x == 0 &&
               "AnimationInfo sprite_size x does not match sprite sheet width");
        assert(this->sprite.height % this->info.sprite_size.y == 0 &&
               "AnimationInfo sprite_size y does not match sprite sheet height");
    }

    void play(int begin_frame = 0) {
        if (this->playing) return;

        assert(begin_frame <= info.frame_count && "Can't select a begin frame that's higher than the frame count");
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

    void draw(Rectangle destination, bool flip_x = false) const {
        Rectangle source = {.x = (float)this->info.sprite_size.x * this->current_frame,
                            .y = (float)this->info.sprite_size.y * this->current_frame,
                            .width = (float)this->info.sprite_size.x,
                            .height = (float)this->info.sprite_size.y};
        if (flip_x) source.width = -source.width;

        DrawTexturePro(this->sprite, source, destination, Vector2{0, 0}, 0, WHITE);
    }
};
