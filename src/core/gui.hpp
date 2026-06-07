#pragma once

#include "core/data.hpp"
#include "raylib.h"
#include <optional>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

const std::string NONE_ID = "NO_ID_SELECTED";

struct UI {
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

    enum class ElementType {
        CONTAINER,
        BUTTON,
        TEXT,
    };

    struct ButtonColor {
        Color border = BLACK;
        Color background = WHITE;
        Color text = BLACK;
    };

    struct ElementStyle {
        LayoutDirection direction = LayoutDirection::Vertical;
        JustifyContent justify_content = JustifyContent::START;
        int font_size = 12;
        int padding = 0;

        ButtonColor color = {};
        ButtonColor color_hover = {};
        ButtonColor color_active = {};
    };

    using ElementId = std::string;

    struct Element {
        ElementId id;
        ElementType type;
        Vec2 position = {};
        Vec2 container_size = {};
        Vec2 content_size = {};
        ElementStyle style = {};

        std::string text = "";

        std::vector<Element> children = {};
    };

    Vec2 top_left;

    std::unordered_map<ElementId, Rect> previous_render_elements;
    std::vector<Element> current_render_elements;
    std::stack<Element> elements;
    ElementId hot = NONE_ID;
    ElementId active = NONE_ID;

    bool building = false;

    void begin_ui(Vec2 position);
    void end_ui();

    void begin_layout(ElementId id, std::optional<Vec2> size, ElementStyle style);
    void end_layout();

    bool button(ElementId id, std::optional<Vec2> size, std::string text, ElementStyle style);

    void text(ElementId id, std::string text, ElementStyle style);
};
