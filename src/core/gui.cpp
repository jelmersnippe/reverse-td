#include "core/gui.hpp"
#include "raylib.h"
#include <cassert>
#include <optional>
#include <unordered_map>

void draw_element(UI* ui, const UI::Element& element) {
    switch (element.type) {
        case UI::ElementType::CONTAINER:
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

            DrawRectangle(element.position.x, element.position.y, element.content_size.x, element.content_size.y,
                          color.background);
            DrawRectangleLines(element.position.x, element.position.y, element.content_size.x, element.content_size.y,
                               color.border);
            const int text_width = MeasureText(element.text.c_str(), element.style.font_size);
            DrawText(element.text.c_str(), element.position.x + element.content_size.x / 2 - text_width / 2,
                     element.position.y + element.content_size.y / 2 - element.style.font_size / 2,
                     element.style.font_size, color.text);
            break;
        }
    }
}

void UI::begin_ui(Vec2 position) {
    assert(!this->building && "Already building the UI. Can't call begin_ui(). Make sure to call end_ui().");

    this->building = true;
    this->top_left = position;
}

void UI::end_ui() {
    assert(this->elements.empty() &&
           "Can't end_ui() with remaining elements. Makes sure to call end for every element.");
    assert(this->building && "Not building the UI. Can't call end_ui(). Make sure to call begin_ui().");

    std::unordered_map<ElementId, Rect> element_rects = {};

    for (const Element& element : this->current_render_elements) {
        draw_element(this, element);

        element_rects[element.id] = Rect{.position = element.position, .size = element.container_size};
    }

    this->previous_render_elements = element_rects;
    this->current_render_elements.clear();
    this->building = false;
}

void UI::begin_layout(ElementId id, std::optional<Vec2> size, ElementStyle style) {
    assert(this->building && "Not building the UI. Can't call begin_layout(). Make sure to call begin_ui().");

    Vec2 calculated_size = {};
    if (size.has_value()) { calculated_size = size.value(); }
    this->elements.push(
        Element{.id = id, .type = ElementType::CONTAINER, .container_size = calculated_size, .style = style});
}

void position_children(UI* ui, UI::Element& element) {
    ui->current_render_elements.push_back(element);

    int offset = 0;
    const Vec2 position = element.position;

    for (UI::Element& child : element.children) {
        switch (element.style.direction) {
            case UI::LayoutDirection::Horizontal: {
                child.position = Vec2{.x = position.x + offset, .y = position.y};
                offset += child.container_size.x;
                break;
            }
            case UI::LayoutDirection::Vertical: {
                child.position = Vec2{.x = position.x, .y = position.y + offset};
                offset += child.container_size.y;
                break;
            }
        }
        position_children(ui, child);
    }
}

void UI::end_layout() {
    assert(!this->elements.empty() && "No more layouts to pop");

    Element layout_to_finish = this->elements.top();
    assert(layout_to_finish.type == ElementType::CONTAINER && "Can't call end_layout for a non layout element.");
    this->elements.pop();

    Vec2 size_to_add = Vec2{};
    for (Element& element : layout_to_finish.children) {
        switch (layout_to_finish.style.direction) {
            case UI::LayoutDirection::Horizontal: {
                size_to_add.x += element.container_size.x;
                const int current_size_y = size_to_add.y;
                if (element.container_size.y > current_size_y)
                    size_to_add.y = element.container_size.y;
                break;
            }
            case UI::LayoutDirection::Vertical: {
                size_to_add.y += element.container_size.y;
                const int current_size_x = size_to_add.x;
                if (element.container_size.x > current_size_x)
                    size_to_add.x = element.container_size.x;
                break;
            }
        }
    }

    layout_to_finish.content_size = Vec2{.x = layout_to_finish.content_size.x + size_to_add.x,
                                         .y = layout_to_finish.content_size.y + size_to_add.y};

    if (layout_to_finish.container_size.x == 0 && layout_to_finish.container_size.y == 0)
        layout_to_finish.container_size = layout_to_finish.content_size;

    if (this->elements.empty()) {
        position_children(this, layout_to_finish);
        return;
    };

    this->elements.top().children.push_back(layout_to_finish);
}

// TODO: Make work for other shapes
bool get_and_update_ui_state(UI* ui, UI::ElementId id) {
    bool result = false;

    if (id == ui->active && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        if (id == ui->hot) result = true;

        ui->active = NONE_ID;
    } else if (id == ui->hot && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        ui->active = id;
    }

    // Only check for hot if nothing is active or this is the active elements
    if (ui->active == NONE_ID || ui->active == id || ui->hot == id) {
        auto rect_it = ui->previous_render_elements.find(id);
        if (rect_it == ui->previous_render_elements.end()) return false;

        Rect rect = rect_it->second;

        auto mouse_pos = GetMousePosition();
        if (CheckCollisionPointRec(mouse_pos, Rectangle{
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

bool UI::button(ElementId id, std::optional<Vec2> size, std::string text, ElementStyle style) {
    bool result = get_and_update_ui_state(this, id);

    const int text_width = MeasureText(text.c_str(), style.font_size);

    Vec2 calculated_size;
    if (size.has_value()) {
        calculated_size = size.value();
    } else {
        calculated_size = Vec2{.x = text_width + style.padding, .y = style.font_size + style.padding};
    }

    Element element = Element{.id = id,
                              .type = ElementType::BUTTON,
                              .container_size = calculated_size,
                              .content_size = calculated_size,
                              .style = style,
                              .text = text};

    this->elements.top().children.push_back(element);

    return result;
}

void UI::text(ElementId id, std::string text, ElementStyle style) {
    const int text_width = MeasureText(text.c_str(), style.font_size);

    Vec2 size = Vec2{.x = text_width, .y = style.font_size};

    Element element = Element{.id = id,
                              .type = ElementType::TEXT,
                              .container_size = size,
                              .content_size = size,
                              .style = style,
                              .text = text};

    this->elements.top().children.push_back(element);
}
