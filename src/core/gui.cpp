#include "core/gui.hpp"
#include "raylib.h"
#include <cassert>
#include <unordered_map>

const float HOLD_THRESHOLD = 0.5f;

bool get_and_update_ui_state(UI* ui, UI::ElementId id, bool hold) {
    bool result = false;

    if (id == ui->active) {
        ui->active_for += GetFrameTime();
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            // If mouse went up and hot
            if (id == ui->hot) result = true;

            ui->active = NONE_ID;
            ui->active_for = 0.0f;
        } else if (hold && ui->active_for >= HOLD_THRESHOLD && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            // If hold is allowed, mouse is down for HOLD_THRESHOLD and hot
            if (id == ui->hot) result = true;
        }
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

// TODO: Make work for other shapes
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

        assert(element_rects.find(element.id) == element_rects.end() && "Can not have duplicate element ids.");

        element_rects[element.id] = Rect{.position = element.position, .size = element.container_size};
    }

    this->previous_render_elements = element_rects;
    this->current_render_elements.clear();
    this->building = false;
}

void position_children(UI* ui, UI::Element& element) {
    ui->current_render_elements.push_back(element);

    Vec2 content_top_left = element.position;
    Vec2 available_container = element.container_size;
    if (element.style.padding != INVALID_INT) {
        content_top_left =
            Vec2{.x = content_top_left.x + element.style.padding, .y = content_top_left.y + element.style.padding};
        available_container = Vec2{
            .x = available_container.x - element.style.padding * 2,
            .y = available_container.y - element.style.padding * 2,
        };
    }

    int gap = 0;
    if (element.style.gap != INVALID_INT) gap = element.style.gap;

    Vec2 content_offset = {};
    switch (element.style.justify_content) {
        case UI::JustifyContent::CENTER:
            content_offset = Vec2{
                .x = (available_container.x - element.content_size.x) / 2,
                .y = (available_container.y - element.content_size.y) / 2,
            };
            break;
        case UI::JustifyContent::END:
            content_offset = Vec2{
                .x = available_container.x - element.content_size.x,
                .y = available_container.y - element.content_size.y,
            };
            break;
        default:
            break;
    }

    for (UI::Element& child : element.children) {
        Vec2 align_offset = {};
        switch (element.style.align_items) {
            case UI::AlignItems::START:
                break;
            case UI::AlignItems::CENTER:
                align_offset = Vec2{.x = (available_container.x - child.container_size.x) / 2,
                                    .y = (available_container.y - child.container_size.y) / 2};
                break;
            case UI::AlignItems::END:
                align_offset = Vec2{.x = available_container.x - child.container_size.x,
                                    .y = available_container.y - child.container_size.y};
                break;
        }

        // TODO: Implement
        Vec2 justify_offset = {};
        switch (element.style.justify_content) {
            case UI::JustifyContent::SPACE_BETWEEN:
            case UI::JustifyContent::SPACE_AROUND:
            case UI::JustifyContent::SPACE_EVENLY:
                break;
            default:
                break;
        }

        switch (element.style.direction) {
            case UI::LayoutDirection::Horizontal: {
                child.position = Vec2{.x = content_top_left.x + justify_offset.x + content_offset.x,
                                      .y = content_top_left.y + align_offset.y};
                content_offset.x += child.container_size.x + gap;
                break;
            }
            case UI::LayoutDirection::Vertical: {
                child.position = Vec2{.x = content_top_left.x + align_offset.x,
                                      .y = content_top_left.y + justify_offset.y + content_offset.y};
                content_offset.y += child.container_size.y + gap;
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

bool UI::begin_button(ElementId id, ElementStyle style, bool hold) {
    bool result = get_and_update_ui_state(this, id, hold);

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
