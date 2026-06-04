#pragma once

#include "core/data.hpp"
#include "raylib.h"
#include <stack>
#include <string>

struct UI {
    enum class LayoutDirection {
        Horizontal,
        Vertical
    };

    struct Layout {
        LayoutDirection direction = LayoutDirection::Vertical;

        Vec2 position;
        Vec2 size = {};
    };

    std::stack<Layout> layouts;

    void begin_layout(UI ui, LayoutDirection direction);
    void end_layout(UI ui);

    bool button(UI ui, Vec2 size, std::string text, int font_size, Color color);
    void text(UI ui, std::string text, int font_size, Color color);
};
