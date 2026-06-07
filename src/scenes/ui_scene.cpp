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

    ui.begin_ui(Vec2{});

    // START - RED
    ui.begin_layout("layout_wrapper", Vec2{.x = SCREEN_WIDTH, .y = SCREEN_HEIGHT},
                    {.direction = UI::LayoutDirection::Vertical,
                     .justify_content = UI::JustifyContent::END,
                     .color = {.border = RED}});

    // START - GREEN
    ui.begin_layout("layout_hz", std::nullopt,
                    {.direction = UI::LayoutDirection::Horizontal, .color = {.border = GREEN}});
    if (ui.button("Button_1", std::nullopt, "Button 1 with extremely long extra text",
                  {.font_size = 20,
                   .padding = 20,
                   .color = {BLACK, WHITE, BLACK},
                   .color_hover = {BLACK, GRAY, WHITE},
                   .color_active = {BLACK, DARKGRAY, WHITE}})) {
        std::cout << "Button 1 clicked!" << std::endl;
    }
    ui.button("Button_2", std::nullopt, "Button 2",
              {.font_size = 12,
               .padding = 75,
               .color = {BLACK, WHITE, BLACK},
               .color_hover = {BLACK, GRAY, WHITE},
               .color_active = {BLACK, DARKGRAY, WHITE}});
    ui.text("Text0", "Random text", {});

    // START - BLUE
    ui.begin_layout("layout_vert1", std::nullopt,
                    {.direction = UI::LayoutDirection::Vertical, .color = {.border = BLUE}});
    ui.button("Button_5", std::nullopt, "Button 5",
              {.padding = 20,
               .color = {BLACK, WHITE, BLACK},
               .color_hover = {BLACK, GRAY, WHITE},
               .color_active = {BLACK, DARKGRAY, WHITE}});
    ui.end_layout();
    // END - BLUE

    ui.button("Button_3", Vec2{.x = 200, .y = 20}, "Button 3",
              {.padding = 8,
               .color = {BLACK, WHITE, BLACK},
               .color_hover = {BLACK, GRAY, WHITE},
               .color_active = {BLACK, DARKGRAY, WHITE}});
    ui.text("Text1", "Text here!", {});
    ui.end_layout();
    // END - GREEN

    // START - ORANGE
    ui.begin_layout("layout_vert2", std::nullopt,
                    {.direction = UI::LayoutDirection::Vertical, .color = {.border = ORANGE}});
    ui.button("Button_7", Vec2{.x = 200, .y = 20}, "Button 7",
              {.font_size = 12,
               .padding = 8,
               .color = {BLACK, WHITE, BLACK},
               .color_hover = {BLACK, GRAY, WHITE},
               .color_active = {BLACK, DARKGRAY, WHITE}});
    ui.button("Button_8", Vec2{.x = 200, .y = 20}, "Button 8",
              {.font_size = 12,
               .padding = 8,
               .color = {BLACK, WHITE, BLACK},
               .color_hover = {BLACK, GRAY, WHITE},
               .color_active = {BLACK, DARKGRAY, WHITE}});
    ui.button("Button_9", Vec2{.x = 200, .y = 20}, "Button 9",
              {.font_size = 12,
               .padding = 8,
               .color = {BLACK, WHITE, BLACK},
               .color_hover = {BLACK, GRAY, WHITE},
               .color_active = {BLACK, DARKGRAY, WHITE}});
    ui.text("Text2", "Text here again!", {.font_size = 12});
    ui.end_layout();
    // END - ORANGE
    ui.end_layout();
    // END - RED

    ui.end_ui();
}
} // namespace

const Scene UI_SCENE = {.name = "UI", .init = nullptr, .update = nullptr, .draw = Draw, .destroy = nullptr};
