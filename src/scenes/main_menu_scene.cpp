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
const UI::ElementStyle BUTTON_STYLE = {
    .font_size = 24,
    .padding = 8,
    .color = {.border = BLACK, .background = WHITE, .text = BLACK},
    .color_hover = {.border = BLACK, .background = GRAY, .text = WHITE},
    .color_active = {.border = BLACK, .background = DARKGRAY, .text = WHITE},
};
UI ui = {};

void Draw(GameState& state) {
    ClearBackground(GRAY);

    ui.begin_ui(Vec2{});
    ui.begin_layout("layout_main_menu", Vec2{.x = SCREEN_WIDTH, .y = SCREEN_HEIGHT},
                    {.direction = UI::LayoutDirection::Vertical});

    ui.text("txt_title", "REVERSE TIDDY", {.font_size = 40});

    if (ui.button("btn_start", Vec2{.x = 400, .y = 100}, "Start", BUTTON_STYLE))
        SCENE_MANAGER.SetScene(state, GAME_SCENE);
    if (ui.button("btn_test", Vec2{.x = 400, .y = 100}, "Test", BUTTON_STYLE))
        SCENE_MANAGER.SetScene(state, TEST_SCENE);
    if (ui.button("btn_ui", Vec2{.x = 400, .y = 100}, "UI", BUTTON_STYLE)) SCENE_MANAGER.SetScene(state, UI_SCENE);
    if (ui.button("btn_quit", Vec2{.x = 400, .y = 100}, "Quit", BUTTON_STYLE)) state.should_exit = true;

    ui.end_layout();
    ui.end_ui();
}
} // namespace

const Scene MAIN_MENU_SCENE = {
    .name = "Main Menu", .init = nullptr, .update = nullptr, .draw = Draw, .destroy = nullptr};
