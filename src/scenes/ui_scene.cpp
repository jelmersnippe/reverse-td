#include "scenes/ui_scene.hpp"

#include "core/gui.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raylib.h"
#include <iostream>
#include <optional>

namespace {
UI ui = {};

void Draw(GameState& state) {
    ClearBackground(GRAY);

    ui.begin_ui();

    ui.begin_layout(Vec2{.x = SCREEN_WIDTH, .y = SCREEN_HEIGHT}, Vec2{0, 0},
                    {.direction = UI::LayoutDirection::Vertical, .justify_content = UI::JustifyContent::END});

    ui.begin_layout(Vec2{}, std::nullopt, {UI::LayoutDirection::Horizontal});
    if (ui.button("Button_1", std::nullopt, "Button 1 with extremely long extra text",
                  {.padding = 20,
                   .font_size = 20,
                   .color = {BLACK, WHITE, BLACK},
                   .hover_color = {BLACK, GRAY, WHITE},
                   .active_color = {BLACK, DARKGRAY, WHITE}})) {
        std::cout << "Button 1 clicked!" << std::endl;
    }
    ui.button("Button_2", std::nullopt, "Button 2",
              {.padding = 75,
               .font_size = 12,
               .color = {BLACK, WHITE, BLACK},
               .hover_color = {BLACK, GRAY, WHITE},
               .active_color = {BLACK, DARKGRAY, WHITE}});
    ui.text("Text0", "Random text", 12, BLACK);

    ui.begin_layout(Vec2{}, std::nullopt, {UI::LayoutDirection::Vertical});
    ui.button("Button_5", std::nullopt, "Button 5",
              {.padding = 20,
               .font_size = 12,
               .color = {BLACK, WHITE, BLACK},
               .hover_color = {BLACK, GRAY, WHITE},
               .active_color = {BLACK, DARKGRAY, WHITE}});
    ui.end_layout();

    ui.button("Button_3", Vec2{.x = 200, .y = 20}, "Button 3",
              {.padding = 8,
               .font_size = 12,
               .color = {BLACK, WHITE, BLACK},
               .hover_color = {BLACK, GRAY, WHITE},
               .active_color = {BLACK, DARKGRAY, WHITE}});
    ui.text("Text1", "Text here!", 12, BLACK);
    ui.end_layout();

    ui.begin_layout(Vec2{}, std::nullopt, {UI::LayoutDirection::Vertical});
    ui.button("Button_7", Vec2{.x = 200, .y = 20}, "Button 7",
              {.padding = 8,
               .font_size = 12,
               .color = {BLACK, WHITE, BLACK},
               .hover_color = {BLACK, GRAY, WHITE},
               .active_color = {BLACK, DARKGRAY, WHITE}});
    ui.button("Button_8", Vec2{.x = 200, .y = 20}, "Button 8",
              {.padding = 8,
               .font_size = 12,
               .color = {BLACK, WHITE, BLACK},
               .hover_color = {BLACK, GRAY, WHITE},
               .active_color = {BLACK, DARKGRAY, WHITE}});
    ui.button("Button_9", Vec2{.x = 200, .y = 20}, "Button 9",
              {.padding = 8,
               .font_size = 12,
               .color = {BLACK, WHITE, BLACK},
               .hover_color = {BLACK, GRAY, WHITE},
               .active_color = {BLACK, DARKGRAY, WHITE}});
    ui.text("Text2", "Text here again!", 12, BLACK);
    ui.end_layout();
    ui.end_layout();

    ui.end_ui();
}
} // namespace

const Scene UI_SCENE = {.name = "UI", .init = nullptr, .update = nullptr, .draw = Draw, .destroy = nullptr};
