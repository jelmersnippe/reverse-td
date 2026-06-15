#pragma once

#include "core/asset_manager.hpp"
#include "core/data.hpp"
#include "raylib.h"
#include <string>
#include <utility>

struct FlipInfo {
    bool x;
    bool y;
};

struct SpriteInfo {
    std::string name;
    Vec2 size;
    int frame = 0;
    FlipInfo should_flip;

    SpriteInfo(std::string name, Vec2 size, int frame = 0, FlipInfo should_flip = {.x = false, .y = false})
        : name(std::move(name)), size(size), frame(frame), should_flip(should_flip) {}
};

inline void render_sprite(SpriteInfo info, Vec2F center, Vec2F size, float angle = 0, Color color = WHITE) {
    Texture2D sprite = get_sprite(info.name);
    Vec2 sprite_top_left = info.size * info.frame;
    Rectangle source = {.x = (float)sprite_top_left.x,
                        .y = (float)sprite_top_left.y,
                        .width = (float)info.size.x,
                        .height = (float)info.size.y};

    Rectangle dest = {.x = center.x, .y = center.y, .width = size.x, .height = size.y};
    Vector2 origin = {size.x * 0.5f, size.y * 0.5f};

    if (info.should_flip.x) { source.width = -info.size.x; }
    if (info.should_flip.y) { source.height = -info.size.y; }

    DrawTexturePro(sprite, source, dest, origin, angle, color);
}

inline void render_rectangle(Vec2F center, Vec2F size, Color color, bool outline_only = false) {
    Vec2F top_left = center - size * 0.5f;
    if (outline_only) {
        DrawRectangleLines(top_left.x, top_left.y, size.x, size.y, color);
    } else {
        DrawRectangle(top_left.x, top_left.y, size.x, size.y, color);
    }
}
