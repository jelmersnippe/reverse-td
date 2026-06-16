#include "scenes/ui_scene.hpp"

#include "core/gui.hpp"
#include "game_state.hpp"
#include "globals.hpp"

namespace {
const UI::ElementStyle BUTTON_STYLE = {.direction = UI::LayoutDirection::Vertical,
                                       .justify_content = UI::JustifyContent::CENTER,
                                       .align_items = UI::AlignItems::CENTER,
                                       .font_size = 12,
                                       .padding = 8,
                                       .width = 100,
                                       .color = {BLACK, WHITE, BLACK},
                                       .color_hover = {BLACK, GRAY, WHITE},
                                       .color_active = {BLACK, DARKGRAY, WHITE}};
UI ui = {};

void horizontal_layout1() {
    const std::string prefix = "layout_hz1";
    // START - GREEN
    ui.begin_layout(prefix, {.direction = UI::LayoutDirection::Horizontal, .gap = 20, .color = {.border = GREEN}});

    // START - ORANGE
    ui.begin_layout(prefix + "layout_vert1", {.direction = UI::LayoutDirection::Vertical,
                                              .justify_content = UI::JustifyContent::START,
                                              .gap = 8,
                                              .height = 200,
                                              .color = {.border = ORANGE}});
    ui.begin_button(prefix + "btn_1_1", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_1_1", "Button 1", {});
    ui.end_button();

    ui.begin_button(prefix + "btn_1_2", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_1_2", "Button 2", {});
    ui.end_button();

    ui.begin_button(prefix + "btn_1_3", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_1_3", "Button 3", {});
    ui.end_button();
    ui.begin_button(prefix + "btn_1_4", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_1_4", "Button 4", {});
    ui.end_button();

    ui.end_layout();
    // END - ORANGE

    // START - PINK
    ui.begin_layout(prefix + "layout_vert2", {.direction = UI::LayoutDirection::Vertical,
                                              .justify_content = UI::JustifyContent::CENTER,
                                              .gap = 8,
                                              .height = 200,
                                              .color = {.border = PINK}});
    ui.begin_button(prefix + "btn_2_1", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_2_1", "Button 1", {});
    ui.end_button();

    ui.begin_button(prefix + "btn_2_2", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_2_2", "Button 2", {});
    ui.end_button();

    ui.begin_button(prefix + "btn_2_3", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_2_3", "Button 3", {});
    ui.end_button();

    ui.end_layout();
    // END - PINK

    // START - BLUE
    ui.begin_layout(prefix + "layout_vert3", {.direction = UI::LayoutDirection::Vertical,
                                              .justify_content = UI::JustifyContent::END,
                                              .gap = 8,
                                              .height = 200,
                                              .color = {.border = BLUE}});
    ui.begin_button(prefix + "btn_3_1", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_3_1", "Button 1", {});
    ui.end_button();

    ui.begin_button(prefix + "btn_3_2", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_3_2", "Button 2", {});
    ui.end_button();

    ui.begin_button(prefix + "btn_3_3", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_3_3", "Button 3", {});
    ui.end_button();

    ui.end_layout();
    // END - BLUE

    ui.end_layout();
    // END - GREEN
}

void horizontal_layout2() {
    const std::string prefix = "layout_hz2";

    // START - GREEN
    ui.begin_layout(prefix, {.direction = UI::LayoutDirection::Vertical, .gap = 20, .color = {.border = GREEN}});

    // START - ORANGE
    ui.begin_layout(prefix + "layout_hor1", {.direction = UI::LayoutDirection::Horizontal,
                                             .justify_content = UI::JustifyContent::START,
                                             .gap = 8,
                                             .width = 500,
                                             .color = {.border = ORANGE}});
    ui.begin_button(prefix + "btn_1_1", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_1_1", "Button 1", {});
    ui.end_button();

    ui.begin_button(prefix + "btn_1_2", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_1_2", "Button 2", {});
    ui.end_button();

    ui.begin_button(prefix + "btn_1_3", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_1_3", "Button 3", {});
    ui.end_button();
    ui.begin_button(prefix + "btn_1_4", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_1_4", "Button 4", {});
    ui.end_button();

    ui.end_layout();
    // END - ORANGE

    // START - PINK
    ui.begin_layout(prefix + "layout_hor2", {.direction = UI::LayoutDirection::Horizontal,
                                             .justify_content = UI::JustifyContent::CENTER,
                                             .gap = 8,
                                             .width = 500,
                                             .color = {.border = PINK}});
    ui.begin_button(prefix + "btn_2_1", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_2_1", "Button 1", {});
    ui.end_button();

    ui.begin_button(prefix + "btn_2_2", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_2_2", "Button 2", {});
    ui.end_button();

    ui.begin_button(prefix + "btn_2_3", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_2_3", "Button 3", {});
    ui.end_button();

    ui.end_layout();
    // END - PINK

    // START - BLUE
    ui.begin_layout(prefix + "layout_hor3", {.direction = UI::LayoutDirection::Horizontal,
                                             .justify_content = UI::JustifyContent::END,
                                             .gap = 8,
                                             .width = 500,
                                             .color = {.border = BLUE}});
    ui.begin_button(prefix + "btn_3_1", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_3_1", "Button 1", {});
    ui.end_button();

    ui.begin_button(prefix + "btn_3_2", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_3_2", "Button 2", {});
    ui.end_button();

    ui.begin_button(prefix + "btn_3_3", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_3_3", "Button 3", {});
    ui.end_button();

    ui.end_layout();
    // END - BLUE

    ui.end_layout();
    // END - GREEN
}

void horizontal_layout3() {
    const std::string prefix = "layout_hz3";

    // START - GREEN
    ui.begin_layout(prefix, {.direction = UI::LayoutDirection::Horizontal, .gap = 20, .color = {.border = GREEN}});

    // START - ORANGE
    ui.begin_layout(prefix + "layout_vert1", {.direction = UI::LayoutDirection::Vertical,
                                              .justify_content = UI::JustifyContent::START,
                                              .align_items = UI::AlignItems::START,
                                              .gap = 8,
                                              .width = 200,
                                              .height = 200,
                                              .color = {.border = ORANGE}});
    ui.begin_button(prefix + "btn_1_1", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_1_1", "Button 1", {});
    ui.end_button();

    ui.begin_button(prefix + "btn_1_2", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_1_2", "Button 2", {});
    ui.end_button();

    ui.begin_button(prefix + "btn_1_3", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_1_3", "Button 3", {});
    ui.end_button();
    ui.begin_button(prefix + "btn_1_4", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_1_4", "Button 4", {});
    ui.end_button();

    ui.end_layout();
    // END - ORANGE

    // START - PINK
    ui.begin_layout(prefix + "layout_vert2", {.direction = UI::LayoutDirection::Vertical,
                                              .justify_content = UI::JustifyContent::CENTER,
                                              .align_items = UI::AlignItems::CENTER,
                                              .gap = 8,
                                              .width = 200,
                                              .height = 200,
                                              .color = {.border = PINK}});
    ui.begin_button(prefix + "btn_2_1", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_2_1", "Button 1", {});
    ui.end_button();

    ui.begin_button(prefix + "btn_2_2", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_2_2", "Button 2", {});
    ui.end_button();

    ui.begin_button(prefix + "btn_2_3", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_2_3", "Button 3", {});
    ui.end_button();

    ui.end_layout();
    // END - PINK

    // START - BLUE
    ui.begin_layout(prefix + "layout_vert3", {.direction = UI::LayoutDirection::Vertical,
                                              .justify_content = UI::JustifyContent::END,
                                              .align_items = UI::AlignItems::END,
                                              .gap = 8,
                                              .width = 200,
                                              .height = 200,
                                              .color = {.border = BLUE}});
    ui.begin_button(prefix + "btn_3_1", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_3_1", "Button 1", {});
    ui.end_button();

    ui.begin_button(prefix + "btn_3_2", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_3_2", "Button 2", {});
    ui.end_button();

    ui.begin_button(prefix + "btn_3_3", BUTTON_STYLE);
    ui.text(prefix + "txt_btn_3_3", "Button 3", {});
    ui.end_button();

    ui.end_layout();
    // END - BLUE

    ui.end_layout();
    // END - GREEN
}

void Update(GameState& state) {
    ui.begin_ui();

    // START - RED
    ui.begin_layout("layout_wrapper", {.direction = UI::LayoutDirection::Vertical,
                                       .width = SCREEN_WIDTH,
                                       .height = SCREEN_HEIGHT,
                                       .color = {.border = RED}});

    // START - YELLOW
    ui.begin_layout("layout_vert",
                    {.direction = UI::LayoutDirection::Vertical, .gap = 20, .color = {.border = YELLOW}});

    horizontal_layout1();
    horizontal_layout2();
    horizontal_layout3();

    ui.end_layout();
    // END - YELLOW

    ui.end_layout();
    // END - RED

    ui.end_ui();
}

void Draw(GameState& state) {
    ClearBackground(GRAY);

    ui.draw();
}
} // namespace

const Scene UI_SCENE = {.name = "UI", .init = nullptr, .update = Update, .draw = Draw, .destroy = nullptr};
