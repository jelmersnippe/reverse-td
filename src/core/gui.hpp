#pragma once

#include "core/data.hpp"
#include "raylib.h"
#include <optional>
#include <stack>
#include <string>
#include <unordered_map>

const std::string NONE_ID = "NO_ID_SELECTED";

struct UI {
    struct ElementStyle {};
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

    enum class JustifyContent {
        START,
        END,
        CENTER,
        SPACE_BETWEEN,
        SPACE_AROUND,
        SPACE_EVENLY
    };

    struct LayoutStyle {
        LayoutDirection direction = LayoutDirection::Vertical;
    };

    struct Layout {
        LayoutStyle style;

        Vec2 position = {};
        Vec2 size;
    };

    std::unordered_map<ElementId, Rect> previous_render_elements;
    std::unordered_map<ElementId, Rect> current_render_elements;
    std::stack<Layout> layouts;
    ElementId hot = NONE_ID;
    ElementId active = NONE_ID;

    bool building = false;

    void begin_ui();
    void end_ui();

    void begin_layout(Vec2 size, std::optional<Vec2> position, LayoutStyle style);
    void end_layout();

    bool button(ElementId id, Vec2 size, std::string text, ButtonStyle);
    void text(ElementId id, std::string text, int font_size, Color color);
};
