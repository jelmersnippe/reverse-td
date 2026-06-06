#include "core/gui.hpp"
#include "raylib.h"
#include <cassert>

Vec2 get_next_position(UI* ui) {
    if (ui->layouts.empty()) return Vec2{};

    UI::Layout& current_layout = ui->layouts.top();

    switch (current_layout.direction) {
        case UI::LayoutDirection::Horizontal:
            return Vec2{.x = current_layout.position.x + current_layout.size.x, .y = current_layout.position.y};
        case UI::LayoutDirection::Vertical:
            return Vec2{.x = current_layout.position.x, .y = current_layout.position.y + current_layout.size.y};
    }
}

void add_size_to_layout(UI::Layout* layout, Vec2 size) {
    Vec2 size_to_add = Vec2{};

    switch (layout->direction) {
        case UI::LayoutDirection::Horizontal: {
            size_to_add.x = size.x;
            const int current_size_y = layout->size.y;
            if (size.y > current_size_y) size_to_add.y = size.y - current_size_y;
            break;
        }
        case UI::LayoutDirection::Vertical: {
            size_to_add.y = size.y;
            const int current_size_x = layout->size.x;
            if (size.x > current_size_x) size_to_add.x = size.x - current_size_x;
            break;
        }
    }

    layout->size = {.x = layout->size.x + size_to_add.x, .y = layout->size.y + size_to_add.y};
}

void UI::begin_layout(LayoutDirection direction) {
    Vec2 position = get_next_position(this);
    this->layouts.push(Layout{.direction = direction, .position = position});
}

void UI::end_layout() {
    assert(!this->layouts.empty() && "No more layouts to pop");

    const Vec2 size = this->layouts.top().size;
    this->layouts.pop();

    if (this->layouts.empty()) return;

    add_size_to_layout(&this->layouts.top(), size);
}

// TODO: Make work for other shapes
bool get_and_update_ui_state(UI* ui, UI::ElementId id, Rectangle rect) {
    bool result = false;

    if (id == ui->active && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        if (id == ui->hot) result = true;

        ui->active = NONE_ID;
    } else if (id == ui->hot && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        ui->active = id;
    }

    // Only check for hot if nothing is active or this is the active elements
    if ((ui->active == NONE_ID || ui->active == id) && CheckCollisionPointRec(GetMousePosition(), rect)) {
        ui->hot = id;
    } else if (ui->hot == id) {
        ui->hot = NONE_ID;
    }

    return result;
}

bool UI::button(ElementId id, Vec2 size, std::string text, ButtonStyle style) {
    assert(!this->layouts.empty() && "No layout to put button in");
    Vec2 position = get_next_position(this);
    bool result = get_and_update_ui_state(
        this, id,
        Rectangle{.x = (float)position.x, .y = (float)position.y, .width = (float)size.x, .height = (float)size.y});

    ButtonColor color = style.color;
    if (this->hot == id) color = style.hover_color;
    if (this->active == id) color = style.active_color;

    DrawRectangle(position.x, position.y, size.x, size.y, color.background);
    DrawRectangleLines(position.x, position.y, size.x, size.y, color.border);
    const int text_width = MeasureText(text.c_str(), style.font_size);
    DrawText(text.c_str(), position.x + size.x / 2 - text_width / 2, position.y + size.y / 2 - style.font_size / 2,
             style.font_size, color.text);

    add_size_to_layout(&this->layouts.top(), size);

    return result;
}

void UI::text(ElementId id, std::string text, int font_size, Color color) {
    assert(!this->layouts.empty() && "No layout to put text in");

    Vec2 position = get_next_position(this);

    const int text_width = MeasureText(text.c_str(), font_size);
    DrawText(text.c_str(), position.x, position.y, font_size, color);

    add_size_to_layout(&this->layouts.top(), Vec2{.x = text_width, .y = font_size});
}
