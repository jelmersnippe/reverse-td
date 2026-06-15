#include "scenes/pause_scene.hpp"
#include "core/gui.hpp"
#include "core/input.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "scenes/game_scene.hpp"
#include "scenes/main_menu_scene.hpp"
#include "systems/scene_manager.hpp"

namespace {
const UI::ElementStyle BUTTON_STYLE = {.justify_content = UI::JustifyContent::CENTER,
                                       .align_items = UI::AlignItems::CENTER,
                                       .padding = 20,
                                       .width = 400,
                                       .color = {.border = BLACK, .background = WHITE, .text = BLACK},
                                       .color_hover = {.border = BLACK, .background = GRAY, .text = WHITE},
                                       .color_active = {.border = BLACK, .background = DARKGRAY, .text = WHITE}};
const UI::ElementStyle BUTTONTEXT_STYLE = {
    .font_size = 24,
};
UI ui = {};

void Update(GameState& state) {
    ui.begin_ui();
    ui.begin_layout("layout_pause", {.direction = UI::LayoutDirection::Vertical,
                                     .justify_content = UI::JustifyContent::CENTER,
                                     .align_items = UI::AlignItems::CENTER,
                                     .gap = 20,
                                     .width = SCREEN_WIDTH,
                                     .height = SCREEN_HEIGHT});

    ui.text("txt_pause", "PAUSED", {.font_size = 40});

    if (ui.begin_button("btn_resume", BUTTON_STYLE)) SCENE_MANAGER.PopScene(state);
    ui.text("txt_resume", "Resume", BUTTONTEXT_STYLE);
    ui.end_button();

    if (ui.begin_button("btn_restart", BUTTON_STYLE)) SCENE_MANAGER.SetScene(state, GAME_SCENE);
    ui.text("txt_restart", "Restart", BUTTONTEXT_STYLE);
    ui.end_button();

    if (ui.begin_button("btn_menu", BUTTON_STYLE)) SCENE_MANAGER.SetScene(state, MAIN_MENU_SCENE);
    ui.text("txt_menu", "Main menu", BUTTONTEXT_STYLE);
    ui.end_button();

    if (ui.begin_button("btn_quit", BUTTON_STYLE)) state.should_exit = true;
    ui.text("txt_quit", "Quit", BUTTONTEXT_STYLE);
    ui.end_button();

    ui.end_layout();
    ui.end_ui();

    if (input_frame.is_key_pressed(Key::Escape) && SCENE_MANAGER.scenes.top().name == PAUSE_SCENE.name) {
        SCENE_MANAGER.PopScene(state);
    }
}

void Draw(GameState& state) {
    ClearBackground(ColorAlpha(GRAY, 0.6));

    ui.draw();
}
} // namespace

const Scene PAUSE_SCENE = {.name = "Pause", .init = nullptr, .update = Update, .draw = Draw, .destroy = nullptr};
