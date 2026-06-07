#include "scenes/ui_scene.hpp"

#include "core/gui.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raylib.h"
#include <iostream>

namespace {
const UI::ElementStyle BUTTON_STYLE = {.font_size = 12,
                                       .padding = 8,
                                       .width = 200,
                                       .height = 20,
                                       .color = {BLACK, WHITE, BLACK},
                                       .color_hover = {BLACK, GRAY, WHITE},
                                       .color_active = {BLACK, DARKGRAY, WHITE}};
UI ui = {};

void Draw(GameState& state) {
    ClearBackground(GRAY);

    ui.begin_ui(Vec2{});

    // START - RED
    ui.begin_layout("layout_wrapper", {.direction = UI::LayoutDirection::Vertical,
                                       .width = SCREEN_WIDTH,
                                       .height = SCREEN_HEIGHT,
                                       .color = {.border = RED}});

    // START - GREEN
    ui.begin_layout("layout_hz", {.direction = UI::LayoutDirection::Horizontal, .color = {.border = GREEN}});
    if (ui.begin_button("Button_1", {.font_size = 20,
                                     .padding = 20,
                                     .color = {BLACK, WHITE, BLACK},
                                     .color_hover = {BLACK, GRAY, WHITE},
                                     .color_active = {BLACK, DARKGRAY, WHITE}})) {
        std::cout << "Button 1 clicked!" << std::endl;
    }
    ui.text("txt_button1", "Button 1 with extremely long extra text", {});
    ui.end_button();

    ui.begin_button("Button_2", {.font_size = 12,
                                 .padding = 75,
                                 .color = {BLACK, WHITE, BLACK},
                                 .color_hover = {BLACK, GRAY, WHITE},
                                 .color_active = {BLACK, DARKGRAY, WHITE}});
    ui.text("txt_button2", "Button 2", {});
    ui.end_button();

    ui.text("Text0", "Random text", {});

    // START - BLUE
    ui.begin_layout("layout_vert1", {.direction = UI::LayoutDirection::Vertical, .color = {.border = BLUE}});
    ui.begin_button("Button_5", {.padding = 20,
                                 .color = {BLACK, WHITE, BLACK},
                                 .color_hover = {BLACK, GRAY, WHITE},
                                 .color_active = {BLACK, DARKGRAY, WHITE}});
    ui.text("txt_button5", "Button 5", {});
    ui.end_button();
    ui.end_layout();
    // END - BLUE

    ui.begin_button("Button_3", BUTTON_STYLE);
    ui.text("txt_button3", "Button 3", {});
    ui.end_button();

    ui.text("Text1", "Text here!", {});
    ui.end_layout();
    // END - GREEN

    // START - ORANGE
    ui.begin_layout("layout_vert2", {.direction = UI::LayoutDirection::Vertical, .color = {.border = ORANGE}});
    ui.begin_button("Button_7", BUTTON_STYLE);
    ui.text("txt_button7", "Button 7", {});
    ui.end_button();

    ui.begin_button("Button_8", BUTTON_STYLE);
    ui.text("txt_button8", "Button 8", {});
    ui.end_button();

    ui.begin_button("Button_9", BUTTON_STYLE);
    ui.text("txt_button9", "Button 9", {});
    ui.end_button();
    ui.text("Text2", "Text here again!", {.font_size = 12});
    ui.end_layout();
    // END - ORANGE
    ui.end_layout();
    // END - RED

    ui.end_ui();
}
} // namespace

const Scene UI_SCENE = {.name = "UI", .init = nullptr, .update = nullptr, .draw = Draw, .destroy = nullptr};
