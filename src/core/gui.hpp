#pragma once

#include "core/data.hpp"
#include "raylib.h"
#include <stack>
#include <string>

const std::string NONE_ID = "NO_ID_SELECTED";

struct UI {
    struct ButtonColor {
        Color border;
        Color background;
        Color text;
    };
    struct ButtonStyle {
        int padding;
        int font_size;
        ButtonColor color;
        ButtonColor hover_color;
        ButtonColor active_color;
    };

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
    ElementId hot = NONE_ID;
    ElementId active = NONE_ID;

    void begin_layout(LayoutDirection direction);
    void end_layout();

    bool button(ElementId id, Vec2 size, std::string text, ButtonStyle);
    void text(ElementId id, std::string text, int font_size, Color color);
};
