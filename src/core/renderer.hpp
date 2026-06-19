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
    Vec2 pivot_point;
    Vec2F scale;
    int frame = 0;
    FlipInfo should_flip;

    SpriteInfo() = default;
    SpriteInfo(std::string name, Vec2 size, Vec2 pivot_point, Vec2F scale, int frame = 0,
               FlipInfo should_flip = {.x = false, .y = false})
        : name(std::move(name)), size(size), pivot_point(pivot_point), scale(scale), frame(frame),
          should_flip(should_flip) {}

    SpriteInfo(std::string name, Vec2 size, Vec2F scale, int frame = 0, FlipInfo should_flip = {.x = false, .y = false})
        : name(std::move(name)), size(size), scale(scale), frame(frame), should_flip(should_flip) {
        this->pivot_point = size * 0.5f;
    }
};

inline void render_sprite(SpriteInfo info, Vec2F center, float angle = 0, Color color = WHITE) {
    Texture2D sprite = get_sprite(info.name);
    Vec2 sprite_top_left = {.x = info.size.x * info.frame, .y = 0};
    Rectangle source = {.x = (float)sprite_top_left.x,
                        .y = (float)sprite_top_left.y,
                        .width = (float)info.size.x,
                        .height = (float)info.size.y};

    Rectangle dest = {.x = center.x,
                      .y = center.y,
                      .width = (float)info.size.x * info.scale.x,
                      .height = (float)info.size.y * info.scale.y};
    Vector2 origin = {.x = static_cast<float>(info.pivot_point.x) * info.scale.x,
                      .y = static_cast<float>(info.pivot_point.y) * info.scale.y};

    if (info.should_flip.x) { source.width = -info.size.x; }
    if (info.should_flip.y) { source.height = -info.size.y; }

    DrawTexturePro(sprite, source, dest, origin, angle, color);
}

inline void render_rectangle(Vec2F position, Vec2F size, Color color, bool centered = true, bool outline_only = false) {
    Vec2F top_left = centered ? position - size * 0.5f : position;
    if (outline_only) {
        DrawRectangleLines(top_left.x, top_left.y, size.x, size.y, color);
    } else {
        DrawRectangle(top_left.x, top_left.y, size.x, size.y, color);
    }
}

inline void render_triangle(Vec2F point_a, Vec2F point_b, Vec2F point_c, Color color) {
    DrawTriangle(point_a.to_raylib(), point_b.to_raylib(), point_c.to_raylib(), color);
}

inline void render_circle(Vec2F center, float radius, Color color, bool outline_only = false) {
    if (outline_only) {
        DrawCircleLines(center.x, center.y, radius, color);
    } else {
        DrawCircle(center.x, center.y, radius, color);
    }
}

inline void render_text(std::string text, Vec2F center, int font_size, Color color) {
    const int text_width = MeasureText(text.c_str(), font_size);
    const Vec2F top_left = center - Vec2F{.x = text_width * 0.5f, .y = font_size * 0.5f};

    DrawText(text.c_str(), top_left.x, top_left.y, font_size, color);
}
