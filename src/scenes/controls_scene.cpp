#include "scenes/controls_scene.hpp"

#include "core/gui.hpp"
#include "game_state.hpp"
#include "globals.hpp"
#include "scenes/game_scene.hpp"
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
UI ui = UI();

void Update(GameState& state) {
    ui.begin_ui();
    ui.begin_layout("layout_main_menu", {.direction = UI::LayoutDirection::Vertical,
                                         .justify_content = UI::JustifyContent::CENTER,
                                         .align_items = UI::AlignItems::CENTER,
                                         .gap = 20,
                                         .width = SCREEN_WIDTH,
                                         .height = SCREEN_HEIGHT});

    ui.text("txt_howto", "How to play", {.font_size = 40});

    ui.text("txt_movement", "Use <W> <A> <S> <D> to move", {.font_size = 24});
    ui.text("txt_shoot", "Use <left mouse> to shoot", {.font_size = 24});
    ui.text("txt_tower", "Use <right mouse> to place a tower", {.font_size = 24});
    ui.text("txt_scrap", "Use <X> to scrap", {.font_size = 24});

    if (ui.begin_button("btn_start", BUTTON_STYLE)) SCENE_MANAGER.SetScene(state, GAME_SCENE);
    ui.text("txt_start", "Start", BUTTONTEXT_STYLE);
    ui.end_button();

    ui.end_layout();
    ui.end_ui();
}

void Draw(GameState&) {
    ClearBackground(GRAY);

    ui.draw();
}
} // namespace

const Scene CONTROLS_SCENE = {.name = "Controls", .init = nullptr, .update = Update, .draw = Draw, .destroy = nullptr};
