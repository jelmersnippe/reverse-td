#include "scenes/pause_scene.hpp"
#include "core/gui.hpp"
#include "game_state.hpp"
#include "raylib.h"
#include "scenes/game_scene.hpp"
#include "scenes/main_menu_scene.hpp"
#include "systems/scene_manager.hpp"

namespace {
const UI::ButtonStyle BUTTON_STYLE = {
    .padding = 8,
    .font_size = 24,
    .color =
        {
            .border = BLACK,
            .background = WHITE,
            .text = BLACK,
        },
    .hover_color =
        {
            .border = BLACK,
            .background = GRAY,
            .text = WHITE,
        },
    .active_color = {.border = BLACK, .background = DARKGRAY, .text = WHITE},
};
UI ui = {};

void Update(GameState& state) {
    for (Input& input : state.inputs) {
        switch (input) {
            case Input::Escape:
                if (SCENE_MANAGER.scenes.top().name == PAUSE_SCENE.name) { SCENE_MANAGER.PopScene(state); }
                break;
            default:
                break;
        }
    }

    state.inputs.clear();
}

void Draw(GameState& state) {
    ClearBackground(ColorAlpha(GRAY, 0.6));

    ui.begin_ui();
    ui.begin_layout(Vec2{}, Vec2{0, 0}, {UI::LayoutDirection::Vertical});

    ui.text("txt_pause", "PAUSED", 40, BLACK);

    if (ui.button("btn_resume", Vec2{.x = 400, .y = 100}, "Resume", BUTTON_STYLE)) SCENE_MANAGER.PopScene(state);
    if (ui.button("btn_restart", Vec2{.x = 400, .y = 100}, "Restart", BUTTON_STYLE))
        SCENE_MANAGER.SetScene(state, GAME_SCENE);
    if (ui.button("btn_menu", Vec2{.x = 400, .y = 100}, "Main Menu", BUTTON_STYLE))
        SCENE_MANAGER.SetScene(state, MAIN_MENU_SCENE);
    if (ui.button("btn_quit", Vec2{.x = 400, .y = 100}, "Quit", BUTTON_STYLE)) state.should_exit = true;

    ui.end_layout();
    ui.end_ui();
}
} // namespace

const Scene PAUSE_SCENE = {.name = "Pause", .init = nullptr, .update = Update, .draw = Draw, .destroy = nullptr};
