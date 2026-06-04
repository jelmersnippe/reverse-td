#include "core/gui.hpp"
#include <cassert>

Vec2 get_next_position(UI ui) {
    assert(!ui.layouts.empty() && "No layout to get next position for");

    UI::Layout& current_layout = ui.layouts.top();

    switch (current_layout.direction) {
        case UI::LayoutDirection::Vertical:
            return Vec2{.x = current_layout.position.x + current_layout.size.x, .y = current_layout.position.y};
        case UI::LayoutDirection::Horizontal:
            return Vec2{.x = current_layout.position.x, .y = current_layout.position.y + current_layout.size.y};
    }
}

void UI::begin_layout(UI ui, LayoutDirection direction) {
    Vec2 position = get_next_position(ui);
    ui.layouts.push(Layout{.direction = direction, .position = position});
}

void UI::end_layout(UI ui) {
    assert(!ui.layouts.empty() && "No more layouts to pop");

    ui.layouts.pop();
}

bool UI::button(UI ui, Vec2 size, std::string text, int font_size, Color color) {
    assert(!ui.layouts.empty() && "No layout to put button in");

    return false;
}

void UI::text(UI ui, std::string text, int font_size, Color color) {
    assert(!ui.layouts.empty() && "No layout to put text in");
}
