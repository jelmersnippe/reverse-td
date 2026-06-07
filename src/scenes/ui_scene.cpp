#include "scenes/ui_scene.hpp"

#include "core/gui.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raylib.h"

namespace {
const UI::ElementStyle BUTTON_STYLE = {.justify_content = UI::JustifyContent::CENTER,
                                       .font_size = 12,
                                       .padding = 8,
                                       .width = 100,
                                       .color = {BLACK, WHITE, BLACK},
                                       .color_hover = {BLACK, GRAY, WHITE},
                                       .color_active = {BLACK, DARKGRAY, WHITE}};
UI ui = {};

void Draw(GameState& state) {
    ClearBackground(GRAY);

    ui.begin_ui(Vec2{});

    // START - RED
    ui.begin_layout("layout_wrapper", {.direction = UI::LayoutDirection::Vertical,
                                       .justify_content = UI::JustifyContent::START,
                                       .width = SCREEN_WIDTH,
                                       .height = SCREEN_HEIGHT,
                                       .color = {.border = RED}});

    // START - YELLOW
    ui.begin_layout("layout_vert", {.direction = UI::LayoutDirection::Vertical, .color = {.border = YELLOW}});

    // START - GREEN
    ui.begin_layout("layout_hz1", {.direction = UI::LayoutDirection::Horizontal, .color = {.border = GREEN}});

    // START - ORANGE
    ui.begin_layout("layout_vert1",
                    {.direction = UI::LayoutDirection::Vertical, .width = 300, .color = {.border = ORANGE}});
    ui.begin_button("btn_1_1", BUTTON_STYLE);
    ui.text("txt_btn_1_1", "Button 1", {});
    ui.end_button();

    ui.begin_button("btn_1_2", BUTTON_STYLE);
    ui.text("txt_btn_1_2", "Button 2", {});
    ui.end_button();

    ui.begin_button("btn_1_3", BUTTON_STYLE);
    ui.text("txt_btn_1_3", "Button 3", {});
    ui.end_button();
    ui.begin_button("btn_1_4", BUTTON_STYLE);
    ui.text("txt_btn_1_4", "Button 4", {});
    ui.end_button();

    ui.end_layout();
    // END - ORANGE

    // START - PINK
    ui.begin_layout("layout_vert2", {.direction = UI::LayoutDirection::Vertical,
                                     .justify_content = UI::JustifyContent::END,
                                     .width = 300,
                                     .color = {.border = PINK}});
    ui.begin_button("btn_2_1", BUTTON_STYLE);
    ui.text("txt_btn_2_1", "Button 1", {});
    ui.end_button();

    ui.begin_button("btn_2_2", BUTTON_STYLE);
    ui.text("txt_btn_2_2", "Button 2", {});
    ui.end_button();

    ui.begin_button("btn_2_3", BUTTON_STYLE);
    ui.text("txt_btn_2_3", "Button 3", {});
    ui.end_button();

    ui.end_layout();
    // END - PINK

    // START - BLUE
    ui.begin_layout("layout_vert3", {.direction = UI::LayoutDirection::Vertical,
                                     .justify_content = UI::JustifyContent::CENTER,
                                     .width = 300,
                                     .color = {.border = BLUE}});
    ui.begin_button("btn_3_1", BUTTON_STYLE);
    ui.text("txt_btn_3_1", "Button 1", {});
    ui.end_button();

    ui.begin_button("btn_3_2", BUTTON_STYLE);
    ui.text("txt_btn_3_2", "Button 2", {});
    ui.end_button();

    ui.begin_button("btn_3_3", BUTTON_STYLE);
    ui.text("txt_btn_3_3", "Button 3", {});
    ui.end_button();

    ui.end_layout();
    // END - BLUE

    ui.end_layout();
    // END - GREEN

    // START - GREEN
    ui.begin_layout("layout_hz2", {.direction = UI::LayoutDirection::Horizontal, .color = {.border = GREEN}});

    // START - ORANGE
    ui.begin_layout("layout_vert2_1",
                    {.direction = UI::LayoutDirection::Horizontal, .height = 200, .color = {.border = ORANGE}});
    ui.begin_button("btn_2_2_1_1", BUTTON_STYLE);
    ui.text("txt_btn_2_2_1_1", "Button 1", {});
    ui.end_button();

    ui.begin_button("btn_2_2_1_2", BUTTON_STYLE);
    ui.text("txt_btn_2_2_1_2", "Button 2", {});
    ui.end_button();

    ui.begin_button("btn_2_2_1_3", BUTTON_STYLE);
    ui.text("txt_btn_2_2_1_3", "Button 3", {});
    ui.end_button();
    ui.begin_button("btn_2_2_1_4", BUTTON_STYLE);
    ui.text("txt_btn_2_2_1_4", "Button 4", {});
    ui.end_button();

    ui.end_layout();
    // END - ORANGE

    // START - PINK
    ui.begin_layout("layout_vert2_2", {.direction = UI::LayoutDirection::Horizontal,
                                       .justify_content = UI::JustifyContent::END,
                                       .height = 200,
                                       .color = {.border = PINK}});
    ui.begin_button("btn_2_2_1", BUTTON_STYLE);
    ui.text("txt_btn_2_2_1", "Button 1", {});
    ui.end_button();

    ui.begin_button("btn_2_2_2", BUTTON_STYLE);
    ui.text("txt_btn_2_2_2", "Button 2", {});
    ui.end_button();

    ui.begin_button("btn_2_2_3", BUTTON_STYLE);
    ui.text("txt_btn_2_2_3", "Button 3", {});
    ui.end_button();

    ui.end_layout();
    // END - PINK

    // START - BLUE
    ui.begin_layout("layout_vert2_3", {.direction = UI::LayoutDirection::Horizontal,
                                       .justify_content = UI::JustifyContent::CENTER,
                                       .height = 200,
                                       .color = {.border = BLUE}});
    ui.begin_button("btn_2_3_1", BUTTON_STYLE);
    ui.text("txt_btn_2_3_1", "Button 1", {});
    ui.end_button();

    ui.begin_button("btn_2_3_2", BUTTON_STYLE);
    ui.text("txt_btn_2_3_2", "Button 2", {});
    ui.end_button();

    ui.begin_button("btn_2_3_3", BUTTON_STYLE);
    ui.text("txt_btn_2_3_3", "Button 3", {});
    ui.end_button();

    ui.end_layout();
    // END - BLUE

    ui.end_layout();
    // END - GREEN

    ui.end_layout();
    // END - YELLOW

    ui.end_layout();
    // END - RED

    ui.end_ui();
}
} // namespace

const Scene UI_SCENE = {.name = "UI", .init = nullptr, .update = nullptr, .draw = Draw, .destroy = nullptr};
