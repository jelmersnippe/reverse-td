#pragma once

#include "core/data.hpp"
#include "raylib.h"
#include <stack>
#include <string>

struct UI {
    using ElementId = std::string;

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
    ElementId hot;
    ElementId active;

    void begin_layout(LayoutDirection direction);
    void end_layout();

    bool button(ElementId id, Vec2 size, std::string text, int font_size, Color color);
    void text(ElementId id, std::string text, int font_size, Color color);
};
