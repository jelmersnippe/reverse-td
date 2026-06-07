#include "core/gui.hpp"
#include "raylib.h"
#include <cassert>
#include <optional>
#include <unordered_map>

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

            DrawRectangle(element.position.x, element.position.y, element.container_size.x, element.container_size.y,
                          color.background);
            DrawRectangleLines(element.position.x, element.position.y, element.container_size.x,
                               element.container_size.y, color.border);
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

void position_children(UI* ui, UI::Element& element) {
    ui->current_render_elements.push_back(element);

    Vec2 offset = {};
    const Vec2 position =
        Vec2{.x = element.position.x + element.style.padding, .y = element.position.y + element.style.padding};

    const Vec2 available_container = Vec2{
        .x = element.container_size.x - element.style.padding * 2,
        .y = element.container_size.y - element.style.padding * 2,
    };

    switch (element.style.justify_content) {
        case UI::JustifyContent::START:
            break;
        case UI::JustifyContent::END:
            offset = Vec2{.x = available_container.x - element.content_size.x,
                          .y = available_container.y - element.content_size.y};
            break;
        case UI::JustifyContent::CENTER:
            offset = Vec2{.x = available_container.x / 2 - element.content_size.x / 2,
                          .y = available_container.y / 2 - element.content_size.y / 2};
            break;
        case UI::JustifyContent::SPACE_BETWEEN:
        case UI::JustifyContent::SPACE_AROUND:
        case UI::JustifyContent::SPACE_EVENLY:
            break;
    }

    for (UI::Element& child : element.children) {
        switch (element.style.direction) {
            case UI::LayoutDirection::Horizontal: {
                child.position = Vec2{.x = position.x + offset.x, .y = position.y};
                offset.x += child.container_size.x;
                break;
            }
            case UI::LayoutDirection::Vertical: {
                child.position = Vec2{.x = position.x, .y = position.y + offset.y};
                offset.y += child.container_size.y;
                break;
            }
        }
        position_children(ui, child);
    }
}

void UI::Element::calculate_size() {
    this->container_size.x = this->style.width;
    this->container_size.y = this->style.height;

    Vec2 content_size = {};
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

    this->content_size =
        Vec2{.x = this->style.padding * 2 + content_size.x, .y = this->style.padding * 2 + content_size.y};

    if (this->container_size.x == INVALID_INT) this->container_size.x = this->content_size.x;
    if (this->container_size.y == INVALID_INT) this->container_size.y = this->content_size.y;
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
        position_children(this, layout);
        return;
    };

    this->elements.top().children.push_back(layout);
}

bool UI::begin_button(ElementId id, ElementStyle style) {
    bool result = get_and_update_ui_state(this, id);

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

    Vec2 size = Vec2{.x = text_width, .y = style.font_size};

    Element element = Element{.id = id,
                              .type = ElementType::TEXT,
                              .container_size = size,
                              .content_size = size,
                              .style = style,
                              .text = text};

    assert(!this->elements.empty() && "A parent element is required to place a text.");

    this->elements.top().children.push_back(element);
}
