#pragma once

#include "core/data.hpp"
#include "raylib.h"
#include <climits>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

const std::string NONE_ID = "NO_ID_SELECTED";
const int INVALID_INT = INT_MAX;

#define TRANSPARENT CLITERAL(Color){0, 0, 0, 0}

const int COLOR_PICKER_WIDTH = 100;
const int COLOR_PICKER_SV_RECT_HEIGHT = 70;
const int COLOR_PICKER_GAP = 10;
const int COLOR_PICKER_HUE_SLIDER_HEIGHT = 20;

struct UI {
    struct HSVRect {
        Color color;
        Vector3 hsv;
        Texture2D texture;
    };

    struct HueStrip {
        bool initialized = false;
        Texture2D texture;
    };

    enum class LayoutDirection {
        Horizontal,
        Vertical
    };

    enum class JustifyContent {
        START,
        END,
        CENTER,
        SPACE_BETWEEN,
    };

    enum class AlignItems {
        START,
        CENTER,
        END
    };

    enum class ElementType {
        CONTAINER,
        BUTTON,
        TEXT,
        COLOR_RECT,
        HUE_STRIP
    };

    struct HoldParams {
        bool hold_enabled = false;
        float hold_threshold = 0.5f;
        bool allow_outside = false;
    };

    struct ButtonColor {
        Color border = TRANSPARENT;
        Color background = WHITE;
        Color text = BLACK;
    };

    struct ElementStyle {
        LayoutDirection direction = LayoutDirection::Vertical;
        JustifyContent justify_content = JustifyContent::START;
        AlignItems align_items = AlignItems::START;
        int font_size = 12;
        int padding = INVALID_INT;
        int gap = INVALID_INT;
        int width = INVALID_INT;
        int height = INVALID_INT;

        ButtonColor color = {};
        ButtonColor color_hover = {};
        ButtonColor color_active = {};
    };

    using ElementId = std::string;

    struct Element {
        ElementId id;
        ElementType type;
        Vec2F position = {};
        Vec2F container_size = {};
        Vec2F content_size = {};
        ElementStyle style = {};

        std::string text = "";
        Color color = {};

        std::vector<Element> children = {};

        void calculate_size();
    };

    Vec2F top_left;

    HueStrip hue_strip = {};
    std::unordered_map<ElementId, HSVRect> hsv_rects = {};

    std::unordered_map<ElementId, Rect> previous_render_elements;
    std::vector<Element> current_render_elements;
    std::stack<Element> elements;
    ElementId hot = NONE_ID;
    ElementId active = NONE_ID;
    float active_for = 0.0f;

    bool building = false;

    void begin_ui();
    void end_ui();

    void begin_layout(ElementId id, ElementStyle style);
    void end_layout();

    bool begin_button(ElementId id, ElementStyle style,
                      HoldParams hold_params = {.hold_enabled = false, .hold_threshold = 0.5f, .allow_outside = false});
    void end_button();

    void text(ElementId id, std::string text, ElementStyle style);

    void color_picker(ElementId id, Color& color);

    UI(Vec2F position = Vec2F{.x = 0, .y = 0}) : top_left(position) {}

    void init_hue_strip();

    void draw();
};
