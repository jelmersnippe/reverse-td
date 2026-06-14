#pragma once

#include "core/gui.hpp"
#include "systems/scene_manager.hpp"

const int INPUT_WIDTH = 200;
const UI::ElementStyle COLLAPSIBLE_BUTTON_STYLE = {.direction = UI::LayoutDirection::Horizontal,
                                                   .justify_content = UI::JustifyContent::SPACE_BETWEEN,
                                                   .align_items = UI::AlignItems::CENTER};

const UI::ElementStyle INPUT_LAYOUT_STYLE = {
    .direction = UI::LayoutDirection::Horizontal, .align_items = UI::AlignItems::CENTER, .gap = 8};

const UI::ElementStyle LABEL_TEXT_STYLE = {.font_size = 16};
const UI::ElementStyle BUTTON_TEXT_STYLE = {.font_size = 16};

const UI::ElementStyle BUTTON_STYLE = {.justify_content = UI::JustifyContent::CENTER,
                                       .align_items = UI::AlignItems::CENTER,
                                       .width = 24,
                                       .height = 24,
                                       .color = {.border = BLACK},
                                       .color_hover =
                                           {
                                               .background = GRAY,
                                           },
                                       .color_active = {
                                           .background = DARKGRAY,
                                       }};

const UI::ElementStyle OPTION_STYLE = {.justify_content = UI::JustifyContent::CENTER,
                                       .align_items = UI::AlignItems::CENTER,
                                       .padding = 8,
                                       .color = {.border = BLACK},
                                       .color_hover =
                                           {
                                               .background = GRAY,
                                           },
                                       .color_active = {
                                           .background = DARKGRAY,
                                       }};

extern const Scene PARTICLE_SCENE;
