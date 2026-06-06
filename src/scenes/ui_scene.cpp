#include "scenes/ui_scene.hpp"

#include "core/gui.hpp"
#include "game_state.hpp"
#include "raylib.h"

namespace {
UI ui = {};

void Draw(GameState& state) {
    ClearBackground(GRAY);

    ui.begin_layout(UI::LayoutDirection::Vertical);
    ui.begin_layout(UI::LayoutDirection::Horizontal);
    ui.button("Button_1", Vec2{.x = 200, .y = 20}, "Button 1", 12, BLACK);
    ui.text("Text0", "Random text", 12, BLACK);
    ui.button("Button_2", Vec2{.x = 200, .y = 20}, "Button 2", 12, BLACK);
    ui.button("Button_3", Vec2{.x = 200, .y = 20}, "Button 3", 12, BLACK);
    ui.text("Text1", "Text here!", 12, BLACK);
    ui.end_layout();
    ui.begin_layout(UI::LayoutDirection::Vertical);
    ui.button("Button_4", Vec2{.x = 200, .y = 20}, "Button 4", 12, BLACK);
    ui.button("Button_5", Vec2{.x = 200, .y = 20}, "Button 5", 12, BLACK);
    ui.button("Button_6", Vec2{.x = 200, .y = 20}, "Button 6", 12, BLACK);
    ui.text("Text2", "Text here again!", 12, BLACK);
    ui.end_layout();
    ui.end_layout();
}
} // namespace

const Scene UI_SCENE = {.name = "UI", .init = nullptr, .update = nullptr, .draw = Draw, .destroy = nullptr};
