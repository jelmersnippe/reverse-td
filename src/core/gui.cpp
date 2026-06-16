#include "core/gui.hpp"
#include "core/data.hpp"
#include "core/input.hpp"
#include "core/key.hpp"
#include "raylib.h"
#include <algorithm>
#include <cassert>
#include <unordered_map>

void UI::init_hue_strip() {
    const int width = COLOR_PICKER_WIDTH;
    const int height = COLOR_PICKER_HUE_SLIDER_HEIGHT;
    Image image = GenImageColor(width, height, BLACK);

    for (float x = 0; x < width; x++) {
        float h = 360.0f * (x / width);

        ImageDrawLine(&image, x, 0, x, height, ColorFromHSV(h, 1.0, 1.0));
    }

    this->hue_strip = {
        .initialized = true,
        .texture = LoadTextureFromImage(image),
    };
    UnloadImage(image);
}

bool get_and_update_ui_state(UI* ui, UI::ElementId id, UI::HoldParams hold_params) {
    bool result = false;

    if (id == ui->active) {
        ui->active_for += GetFrameTime();
        if (input_frame.is_mouse_released(Mouse::Left)) {
            // If mouse went up and hot
            if (id == ui->hot) result = true;

            ui->active = NONE_ID;
            ui->active_for = 0.0f;
        } else if (hold_params.hold_enabled && ui->active_for >= hold_params.hold_threshold &&
                   input_frame.is_mouse_down(Mouse::Left)) {
            // If hold is allowed, mouse is down for HOLD_THRESHOLD and hot
            if (id == ui->hot) result = true;
        }
        input_frame.consume_mouse(Mouse::Left);
    } else if (id == ui->hot && input_frame.is_mouse_pressed(Mouse::Left)) {
        input_frame.consume_mouse(Mouse::Left);
        ui->active = id;
    }

    // Only check for hot if nothing is active or this is the active elements
    if (ui->active == NONE_ID || ui->active == id || ui->hot == id) {
        auto rect_it = ui->previous_render_elements.find(id);
        if (rect_it == ui->previous_render_elements.end()) return false;

        Rect rect = rect_it->second;

        auto mouse_pos = GetMousePosition();
        if ((ui->active == id && hold_params.allow_outside) ||
            CheckCollisionPointRec(mouse_pos, Rectangle{
                                                  .x = (float)rect.position.x,
                                                  .y = (float)rect.position.y,
                                                  .width = (float)rect.size.x,
                                                  .height = (float)rect.size.y,
                                              })) {
            ui->hot = id;
        } else if (ui->hot == id) {
            ui->hot = NONE_ID;
        }
    }

    return result;
}

// TODO: Make work for other shapes
void draw_element(UI* ui, UI::Element& element) {
    switch (element.type) {
        case UI::ElementType::CONTAINER:
            if (!ColorIsEqual(element.style.color.background, TRANSPARENT)) {
                DrawRectangle(element.position.x, element.position.y, element.container_size.x,
                              element.container_size.y, element.style.color.background);
            }
            DrawRectangleLines(element.position.x, element.position.y, element.container_size.x,
                               element.container_size.y, element.style.color.border);
            break;
        case UI::ElementType::TEXT:
            DrawText(element.text.c_str(), element.position.x, element.position.y, element.style.font_size,
                     element.style.color.text);
            break;
        case UI::ElementType::BUTTON: {
            UI::ButtonColor color = element.style.color;
            if (ui->hot == element.id) color = element.style.color_hover;
            if (ui->active == element.id) color = element.style.color_active;

            DrawRectangle(element.position.x, element.position.y, element.container_size.x, element.container_size.y,
                          color.background);
            DrawRectangleLines(element.position.x, element.position.y, element.container_size.x,
                               element.container_size.y, color.border);
            break;
        }

        case UI::ElementType::COLOR_RECT: {
            const Rectangle sv_rect = Rectangle{.x = (float)element.position.x,
                                                .y = (float)element.position.y,
                                                .width = (float)element.content_size.x,
                                                .height = (float)element.content_size.y};

            auto hsv_rect_iterator = ui->hsv_rects.find(element.id);
            assert(hsv_rect_iterator != ui->hsv_rects.end() && "Color picker did not have hsv_rect generated.");

            UI::HSVRect hsv_rect = hsv_rect_iterator->second;

            const Vector3 current_hsv = ColorToHSV(element.color);

            DrawTexture(hsv_rect.texture, element.position.x, element.position.y, WHITE);

            int s_pos = sv_rect.x + (sv_rect.width * current_hsv.y);
            int v_pos = sv_rect.y + sv_rect.height - (sv_rect.height * current_hsv.z);
            DrawCircle(s_pos, v_pos, 5, WHITE);
            DrawCircleLines(s_pos, v_pos, 5, BLACK);

            break;
        }
        case UI::ElementType::HUE_STRIP: {
            const Rectangle hue_rect = Rectangle{.x = (float)element.position.x,
                                                 .y = (float)element.position.y,
                                                 .width = (float)element.content_size.x,
                                                 .height = (float)element.content_size.y};

            assert(ui->hue_strip.initialized && "UI hue strip not initialized.");

            const Vector3 current_hsv = ColorToHSV(element.color);

            DrawTexture(ui->hue_strip.texture, element.position.x,
                        element.position.y + element.content_size.y - COLOR_PICKER_HUE_SLIDER_HEIGHT, WHITE);

            int h_pos = hue_rect.x + (hue_rect.width * (current_hsv.x / 360.0f));
            DrawRectangleLines(h_pos - 2, hue_rect.y, 4, hue_rect.height, WHITE);

            break;
        }
    }
}

void UI::begin_ui() {
    assert(!this->building && "Already building the UI. Can't call begin_ui(). Make sure to call end_ui().");

    this->current_render_elements.clear();
    this->building = true;
}

void UI::end_ui() {
    assert(this->elements.empty() &&
           "Can't end_ui() with remaining elements. Makes sure to call end for every element.");
    assert(this->building && "Not building the UI. Can't call end_ui(). Make sure to call begin_ui().");

    std::unordered_map<ElementId, Rect> element_rects = {};

    for (Element& element : this->current_render_elements) {
        assert(element_rects.find(element.id) == element_rects.end() && "Can not have duplicate element ids.");

        element_rects[element.id] = Rect{.position = element.position, .size = element.container_size};
    }

    this->previous_render_elements = element_rects;
    this->building = false;
}

void UI::draw() {
    for (Element& element : this->current_render_elements) {
        draw_element(this, element);
    }
}

void position_children(UI* ui, UI::Element& element) {
    ui->current_render_elements.push_back(element);

    Vec2F content_top_left = element.position;
    Vec2F available_container = element.container_size;
    if (element.style.padding != INVALID_INT) {
        content_top_left += {.x = (float)element.style.padding, .y = (float)element.style.padding};
        available_container = {
            .x = available_container.x - element.style.padding * 2.0f,
            .y = available_container.y - element.style.padding * 2.0f,
        };
    }

    // TODO: Also do hot/active check and provide border_width option
    if (!ColorIsEqual(element.style.color.border, TRANSPARENT)) {
        Vec2F border_size = {.x = 1, .y = 1};
        content_top_left += border_size;
        available_container -= border_size * 2;
    }

    int gap = 0;
    if (element.style.gap != INVALID_INT) gap = element.style.gap;

    Vec2F content_offset = {};
    Vec2F justify_offset = {};
    switch (element.style.justify_content) {
        case UI::JustifyContent::CENTER:
            content_offset = {
                .x = (available_container.x - element.content_size.x) / 2,
                .y = (available_container.y - element.content_size.y) / 2,
            };
            break;
        case UI::JustifyContent::END:
            content_offset = {
                .x = available_container.x - element.content_size.x,
                .y = available_container.y - element.content_size.y,
            };
            break;
        case UI::JustifyContent::SPACE_BETWEEN:
            if (element.children.size() == 0) break;

            justify_offset = {
                .x = (available_container.x - element.content_size.x) / int(element.children.size() - 1),
                .y = (available_container.y - element.content_size.y) / int(element.children.size() - 1),
            };
            break;
        default:
            break;
    }

    for (UI::Element& child : element.children) {
        Vec2F align_offset = {};
        switch (element.style.align_items) {
            case UI::AlignItems::START:
                break;
            case UI::AlignItems::CENTER:
                align_offset = {.x = (available_container.x - child.container_size.x) / 2,
                                .y = (available_container.y - child.container_size.y) / 2};
                break;
            case UI::AlignItems::END:
                align_offset = {.x = available_container.x - child.container_size.x,
                                .y = available_container.y - child.container_size.y};
                break;
        }

        switch (element.style.direction) {
            case UI::LayoutDirection::Horizontal: {
                child.position = {.x = content_top_left.x + content_offset.x, .y = content_top_left.y + align_offset.y};
                content_offset.x += child.container_size.x + justify_offset.x + gap;

                // Flex 1
                if (child.style.height == INVALID_INT) child.container_size.y = available_container.y;
                break;
            }
            case UI::LayoutDirection::Vertical: {
                child.position = {.x = content_top_left.x + align_offset.x, .y = content_top_left.y + content_offset.y};
                content_offset.y += child.container_size.y + justify_offset.y + gap;

                // Flex 1
                if (child.style.width == INVALID_INT) child.container_size.x = available_container.x;
                break;
            }
        }
        position_children(ui, child);
    }
}

void UI::Element::calculate_size() {
    this->container_size.x = this->style.width;
    this->container_size.y = this->style.height;

    Vec2F content_size = {};
    for (UI::Element& child : this->children) {
        switch (this->style.direction) {
            case UI::LayoutDirection::Horizontal: {
                content_size.x += child.container_size.x;
                const int current_size_y = content_size.y;
                if (child.container_size.y > current_size_y) content_size.y = child.container_size.y;
                break;
            }
            case UI::LayoutDirection::Vertical: {
                content_size.y += child.container_size.y;
                const int current_size_x = content_size.x;
                if (child.container_size.x > current_size_x) content_size.x = child.container_size.x;
                break;
            }
        }
    }

    int gap = 0;
    if (this->style.gap != INVALID_INT) gap = ((int)this->children.size() - 1) * this->style.gap;
    switch (this->style.direction) {
        case UI::LayoutDirection::Horizontal: {
            content_size.x += gap;
            break;
        }
        case UI::LayoutDirection::Vertical: {
            content_size.y += gap;
            break;
        }
    }

    this->content_size = content_size;

    int padding = 0;
    if (this->style.padding != INVALID_INT) padding = this->style.padding;

    if (this->container_size.x == INVALID_INT) this->container_size.x = this->content_size.x + padding * 2;
    if (this->container_size.y == INVALID_INT) this->container_size.y = this->content_size.y + padding * 2;
}

void UI::begin_layout(ElementId id, ElementStyle style) {
    assert(this->building && "Not building the UI. Can't call begin_layout(). Make sure to call begin_ui().");

    this->elements.push(Element{.id = id, .type = ElementType::CONTAINER, .style = style});
}

void UI::end_layout() {
    assert(!this->elements.empty() && "No more elements to pop");

    Element layout = this->elements.top();
    assert(layout.type == ElementType::CONTAINER && "Can't call end_layout for a non layout element.");
    this->elements.pop();

    layout.calculate_size();

    if (this->elements.empty()) {
        layout.position = this->top_left;
        position_children(this, layout);
        return;
    };

    this->elements.top().children.push_back(layout);
}

bool UI::begin_button(ElementId id, ElementStyle style, HoldParams hold_params) {
    bool result = get_and_update_ui_state(this, id, hold_params);

    Element element = Element{.id = id, .type = ElementType::BUTTON, .style = style};

    this->elements.push(element);

    return result;
}

void UI::end_button() {
    assert(!this->elements.empty() && "No more elements to pop");

    Element button = this->elements.top();
    assert(button.type == ElementType::BUTTON && "Can't call end_button for a non button element.");
    this->elements.pop();

    button.calculate_size();

    assert(!this->elements.empty() && "A parent element is required to place a button.");

    this->elements.top().children.push_back(button);
}

void UI::text(ElementId id, std::string text, ElementStyle style) {
    const int text_width = MeasureText(text.c_str(), style.font_size);

    Vec2F size = {.x = (float)text_width, .y = (float)style.font_size};

    Element element = Element{.id = id,
                              .type = ElementType::TEXT,
                              .container_size = size,
                              .content_size = size,
                              .style = style,
                              .text = text};

    assert(!this->elements.empty() && "A parent element is required to place a text.");

    this->elements.top().children.push_back(element);
}

void generate_hsv_texture(UI* ui, const UI::Element& element) {
    const int width = element.content_size.x;
    const int height = element.content_size.y;
    Image image = GenImageColor(width, height, BLACK);

    Vector3 current_hsv = ColorToHSV(element.color);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float s = x / (float)(width - 1);
            float v = 1.0f - y / (float)(height - 1);

            ImageDrawPixel(&image, x, y, ColorFromHSV(current_hsv.x, s, v));
        }
    }

    ui->hsv_rects[element.id] =
        UI::HSVRect{.color = element.color, .hsv = current_hsv, .texture = LoadTextureFromImage(image)};
    UnloadImage(image);
}

void UI::color_picker(ElementId id, Color& color) {
    Vec2F color_rect_size = {.x = COLOR_PICKER_WIDTH, .y = COLOR_PICKER_SV_RECT_HEIGHT};

    Vec2F hue_strip_size = {.x = COLOR_PICKER_WIDTH, .y = COLOR_PICKER_HUE_SLIDER_HEIGHT};
    const std::string rect_id = id + "_rect";
    const std::string strip_id = id + "_strip";

    begin_layout(id + "_layout", {.direction = LayoutDirection::Vertical, .gap = COLOR_PICKER_GAP});

    Element color_rect = Element{.id = rect_id,
                                 .type = ElementType::COLOR_RECT,
                                 .container_size = color_rect_size,
                                 .content_size = color_rect_size,
                                 .color = color};
    Element hue_strip = Element{.id = strip_id,
                                .type = ElementType::HUE_STRIP,
                                .container_size = hue_strip_size,
                                .content_size = hue_strip_size,
                                .color = color};

    if (get_and_update_ui_state(this, rect_id, {.hold_enabled = true, .hold_threshold = 0.0f, .allow_outside = true})) {
        Vector2 mouse_pos = GetMousePosition();

        auto rect_it = this->previous_render_elements.find(rect_id);
        assert(rect_it != this->previous_render_elements.end() && "Color rect used but not found in previous render.");

        Rect rect = rect_it->second;
        Vec2F rect_coords = {.x = std::clamp(mouse_pos.x - rect.position.x, 0.0f, rect.size.x),
                             .y = std::clamp(mouse_pos.y - rect.position.y, 0.0f, rect.size.y)};
        Vec2F sv_value = {
            .x = std::clamp((float)rect_coords.x / (float)color_rect_size.x, 0.0f, 1.0f),
            .y = std::clamp(((float)color_rect_size.y - rect_coords.y) / (float)color_rect_size.y, 0.0f, 1.0f)};

        const Vector3 hsv = ColorToHSV(color);
        const Color new_color = ColorFromHSV(hsv.x, sv_value.x, sv_value.y);
        color.r = new_color.r;
        color.g = new_color.g;
        color.b = new_color.b;
        color.a = new_color.a;
    }
    if (get_and_update_ui_state(this, strip_id,
                                {.hold_enabled = true, .hold_threshold = 0.0f, .allow_outside = true})) {
        Vector2 mouse_pos = GetMousePosition();

        auto strip_it = this->previous_render_elements.find(strip_id);
        assert(strip_it != this->previous_render_elements.end() && "Hue strip used but not found in previous render.");

        Rect strip = strip_it->second;
        int strip_x = std::clamp(mouse_pos.x - strip.position.x, 0.0f, strip.size.x);
        float h_value = std::clamp(((float)strip_x / (float)hue_strip_size.x) * 360.0f, 0.0f, 359.9f);

        const Vector3 hsv = ColorToHSV(color);
        const Color new_color = ColorFromHSV(h_value, hsv.y, hsv.z);
        color.r = new_color.r;
        color.g = new_color.g;
        color.b = new_color.b;
        color.a = new_color.a;
    }

    auto hsv_rect_iterator = this->hsv_rects.find(color_rect.id);

    if (hsv_rect_iterator == this->hsv_rects.end() || !ColorIsEqual(hsv_rect_iterator->second.color, color)) {
        generate_hsv_texture(this, color_rect);
    }

    if (!this->hue_strip.initialized) this->init_hue_strip();

    assert(!this->elements.empty() && "A parent element is required to place a color picker.");

    this->elements.top().children.push_back(color_rect);
    this->elements.top().children.push_back(hue_strip);

    end_layout();
}
