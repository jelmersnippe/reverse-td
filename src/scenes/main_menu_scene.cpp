#include "scenes/main_menu_scene.hpp"
#include "core/gui.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "raylib.h"
#include "scenes/game_scene.hpp"
#include "scenes/test_scene.hpp"
#include "scenes/ui_scene.hpp"
#include "systems/scene_manager.hpp"

namespace {
const UI::ElementStyle BUTTON_STYLE = {.justify_content = UI::JustifyContent::CENTER,
                                       .padding = 20,
                                       .width = 400,
                                       .color = {.border = BLACK, .background = WHITE, .text = BLACK},
                                       .color_hover = {.border = BLACK, .background = GRAY, .text = WHITE},
                                       .color_active = {.border = BLACK, .background = DARKGRAY, .text = WHITE}};
const UI::ElementStyle BUTTONTEXT_STYLE = {
    .font_size = 24,
};
UI ui = {};

void Draw(GameState& state) {
    ClearBackground(GRAY);

    ui.begin_ui(Vec2{});
    ui.begin_layout("layout_main_menu", {.direction = UI::LayoutDirection::Vertical,
                                         .justify_content = UI::JustifyContent::CENTER,
                                         .width = SCREEN_WIDTH,
                                         .height = SCREEN_HEIGHT});

    ui.text("txt_title", "REVERSE TIDDY", {.font_size = 40});

    if (ui.begin_button("btn_start", BUTTON_STYLE)) SCENE_MANAGER.SetScene(state, GAME_SCENE);
    ui.text("txt_start", "Start", BUTTONTEXT_STYLE);
    ui.end_button();
    if (ui.begin_button("btn_test", BUTTON_STYLE)) SCENE_MANAGER.SetScene(state, TEST_SCENE);
    ui.text("txt_test", "Test", BUTTONTEXT_STYLE);
    ui.end_button();
    if (ui.begin_button("btn_ui", BUTTON_STYLE)) SCENE_MANAGER.SetScene(state, UI_SCENE);
    ui.text("txt_ui", "Ui", BUTTONTEXT_STYLE);
    ui.end_button();
    if (ui.begin_button("btn_quit", BUTTON_STYLE)) state.should_exit = true;
    ui.text("txt_quit", "Quit", BUTTONTEXT_STYLE);
    ui.end_button();

    ui.end_layout();
    ui.end_ui();
}
} // namespace

const Scene MAIN_MENU_SCENE = {
    .name = "Main Menu", .init = nullptr, .update = nullptr, .draw = Draw, .destroy = nullptr};
