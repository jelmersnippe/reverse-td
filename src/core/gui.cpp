#include "core/gui.hpp"
#include "raylib.h"
#include <cassert>
#include <optional>

void UI::begin_ui() {
    assert(!this->building && "Already building the UI. Can't call begin_ui(). Make sure to call end_ui().");

    this->building = true;
}

void UI::end_ui() {
    assert(this->layouts.empty() &&
           "Can't end_ui() with remaining layouts. Makes sure to call end_layout() for every layout.");
    assert(this->building && "Not building the UI. Can't call end_ui(). Make sure to call begin_ui().");

    this->previous_render_elements = this->current_render_elements;
    this->current_render_elements.clear();
    this->building = false;
}

Vec2 get_next_position(UI* ui) {
    assert(!ui->layouts.empty() && "No layout to get next position from. Make sure to call begin_layout().");

    UI::Layout& current_layout = ui->layouts.top();

    switch (current_layout.style.direction) {
        case UI::LayoutDirection::Horizontal:
            return Vec2{.x = current_layout.position.x + current_layout.content_size.x, .y = current_layout.position.y};
        case UI::LayoutDirection::Vertical:
            return Vec2{.x = current_layout.position.x, .y = current_layout.position.y + current_layout.content_size.y};
    }
}

void add_size_to_layout(UI::Layout* layout, Vec2 size) {
    Vec2 size_to_add = Vec2{};

    switch (layout->style.direction) {
        case UI::LayoutDirection::Horizontal: {
            size_to_add.x = size.x;
            const int current_size_y = layout->content_size.y;
            if (size.y > current_size_y) size_to_add.y = size.y - current_size_y;
            break;
        }
        case UI::LayoutDirection::Vertical: {
            size_to_add.y = size.y;
            const int current_size_x = layout->content_size.x;
            if (size.x > current_size_x) size_to_add.x = size.x - current_size_x;
            break;
        }
    }

    layout->content_size = {.x = layout->content_size.x + size_to_add.x, .y = layout->content_size.y + size_to_add.y};
}

void UI::begin_layout(Vec2 size, std::optional<Vec2> position, LayoutStyle style) {
    assert(this->building && "Not building the UI. Can't call begin_layout(). Make sure to call begin_ui().");

    Vec2 calculated_position;
    if (position.has_value()) {
        calculated_position = position.value();
    } else {
        calculated_position = get_next_position(this);
    }
    this->layouts.push(Layout{.style = style, .position = calculated_position, .container_size = size});
}

void UI::end_layout() {
    assert(!this->layouts.empty() && "No more layouts to pop");

    Layout layout_to_finish = this->layouts.top();

    if (layout_to_finish.container_size.x == 0 && layout_to_finish.container_size.y == 0) layout_to_finish.container_size = layout_to_finish.content_size;

    this->layouts.pop();

    if (this->layouts.empty()) return;

    add_size_to_layout(&this->layouts.top(), layout_to_finish.container_size);
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

bool UI::button(ElementId id, Vec2 size, std::string text, ButtonStyle style) {
    Vec2 position = get_next_position(this);
    bool result = get_and_update_ui_state(this, id);

    UI::ButtonColor color = style.color;
    if (this->hot == id) color = style.hover_color;
    if (this->active == id) color = style.active_color;

    DrawRectangle(position.x, position.y, size.x, size.y, color.background);
    DrawRectangleLines(position.x, position.y, size.x, size.y, color.border);
    const int text_width = MeasureText(text.c_str(), style.font_size);
    DrawText(text.c_str(), position.x + size.x / 2 - text_width / 2, position.y + size.y / 2 - style.font_size / 2,
             style.font_size, color.text);

    this->current_render_elements[id] = Rect{.position = position, .size = size};
    add_size_to_layout(&this->layouts.top(), size);

    return result;
}

void UI::text(ElementId id, std::string text, int font_size, Color color) {
    Vec2 position = get_next_position(this);

    const int text_width = MeasureText(text.c_str(), font_size);
    DrawText(text.c_str(), position.x, position.y, font_size, color);

    add_size_to_layout(&this->layouts.top(), Vec2{.x = text_width, .y = font_size});
}
