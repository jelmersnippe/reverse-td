#include "scenes/main_menu_scene.hpp"
#include "core/gui.hpp"
#include "game_state.hpp"
#include "raylib.h"
#include "scenes/game_scene.hpp"
#include "scenes/test_scene.hpp"
#include "scenes/ui_scene.hpp"
#include "systems/scene_manager.hpp"

namespace {
const UI::ButtonStyle BUTTON_STYLE = {
    .padding = 8,
    .font_size = 24,
    .color = {.border = BLACK, .background = WHITE, .text = BLACK},
    .hover_color = {.border = BLACK, .background = GRAY, .text = BLACK},
    .active_color = {.border = BLACK, .background = DARKGRAY, .text = BLACK},
};
UI ui = {};

void Draw(GameState& state) {
    ClearBackground(GRAY);

    ui.begin_layout(UI::LayoutDirection::Vertical);

    ui.text("Title", "REVERSE TIDDY", 40, BLACK);

    if (ui.button("btn_start", Vec2{.x = 400, .y = 100}, "Start", BUTTON_STYLE))
        SCENE_MANAGER.SetScene(state, GAME_SCENE);
    if (ui.button("btn_test", Vec2{.x = 400, .y = 100}, "Test", BUTTON_STYLE))
        SCENE_MANAGER.SetScene(state, TEST_SCENE);
    if (ui.button("btn_ui", Vec2{.x = 400, .y = 100}, "UI", BUTTON_STYLE)) SCENE_MANAGER.SetScene(state, UI_SCENE);
    if (ui.button("btn_quit", Vec2{.x = 400, .y = 100}, "Quit", BUTTON_STYLE)) state.should_exit = true;

    ui.end_layout();
}
} // namespace

const Scene MAIN_MENU_SCENE = {
    .name = "Main Menu", .init = nullptr, .update = nullptr, .draw = Draw, .destroy = nullptr};
