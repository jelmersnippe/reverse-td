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

    this->layouts.top().size = {.x = this->layouts.top().size.x + size.x, .y = this->layouts.top().size.y + size.y};
}

bool UI::button(ElementId id, Vec2 size, std::string text, int font_size, Color color) {
    assert(!this->layouts.empty() && "No layout to put button in");

    Vec2 position = get_next_position(this);

    DrawRectangleLines(position.x, position.y, size.x, size.y, BLACK);
    const int text_width = MeasureText(text.c_str(), font_size);
    DrawText(text.c_str(), position.x + size.x / 2 - text_width / 2, position.y + size.y / 2 - font_size / 2, font_size,
             color);

    add_size_to_layout(&this->layouts.top(), size);

    return false;
}

void UI::text(ElementId id, std::string text, int font_size, Color color) {
    assert(!this->layouts.empty() && "No layout to put text in");

    Vec2 position = get_next_position(this);

    const int text_width = MeasureText(text.c_str(), font_size);
    DrawText(text.c_str(), position.x, position.y, font_size, color);

    add_size_to_layout(&this->layouts.top(), Vec2{.x = text_width, .y = font_size});
}
