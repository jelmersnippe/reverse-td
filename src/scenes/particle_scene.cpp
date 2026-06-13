#include "scenes/particle_scene.hpp"

#include "core/data.hpp"
#include "core/gui.hpp"
#include "core/particles.hpp"
#include "format"
#include "raylib.h"
#include <ranges>

namespace {

ParticleSystem particle_system;

ParticleTemplate PARTICLE_TEMPLATE =
    ParticleTemplate({.speed = {.start = {.min = 40, .max = 120}, .end = {.min = 10, .max = 30}},
                      .size = {.start = {.min = 8, .max = 14}, .end = {.min = 0, .max = 4}},
                      .color = {Color(255, 180, 50, 255), Color(40, 40, 40, 0)},
                      .lifetime = {.min = 0.4f, .max = 1.0f},
                      .gravity = {.x = 0, .y = -20}});
Emitter EMITTER = Emitter{.position = {.x = 0, .y = 0},
                          .direction = {.x = 0, .y = -1},
                          .spread = 0.6f,
                          .particle_template = PARTICLE_TEMPLATE,
                          .rate = 80,
                          .duration = 20};

void Update(GameState& state) {
    const Vector2 mouse_pos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        EMITTER.position = {.x = mouse_pos.x, .y = mouse_pos.y};
        EMITTER.particle_template = PARTICLE_TEMPLATE;
        particle_system.play(EMITTER);
    }

    particle_system.update(GetFrameTime());
}

UI ui = UI(Vec2{.x = 25, .y = 25});

const UI::ElementStyle TEXT_STYLE = {.font_size = 16};

const UI::ElementStyle BUTTON_STYLE = {.justify_content = UI::JustifyContent::CENTER,
                                       .align_items = UI::AlignItems::CENTER,
                                       .width = 24,
                                       .height = 24,
                                       .color_hover =
                                           {
                                               .background = GRAY,
                                           },
                                       .color_active = {
                                           .background = DARKGRAY,
                                       }};

void ui_float_variable(std::string label, float& variable) {
    ui.begin_layout(
        "variable_" + label,
        {.direction = UI::LayoutDirection::Horizontal, .align_items = UI::AlignItems::CENTER, .padding = 4, .gap = 8});

    ui.text("txt_" + label, label, {});

    if (ui.begin_button("btn_down_" + label, BUTTON_STYLE, {.hold_enabled = true})) variable -= 1;

    ui.text("btn_txt_down_" + label, "-", TEXT_STYLE);
    ui.end_button();

    ui.text("txt_value_" + label, std::format("{}", variable), TEXT_STYLE);

    if (ui.begin_button("btn_up_" + label, BUTTON_STYLE, {.hold_enabled = true})) variable += 1;
    ui.text("btn_txt_up_" + label, "+", TEXT_STYLE);
    ui.end_button();

    ui.end_layout();
}

void ui_color_variable(std::string label, Color& variable) {
    ui.begin_layout(
        "variable_" + label,
        {.direction = UI::LayoutDirection::Horizontal, .align_items = UI::AlignItems::CENTER, .padding = 4, .gap = 8});

    ui.text("txt_" + label, label, {});

    ui.color_picker("cp_" + label, variable);
    ui.text("text_" + label + "_start",
            std::format("RGBA ({},{},{},{})", variable.r, variable.g, variable.b, variable.a), TEXT_STYLE);

    ui.end_layout();
}

void Draw(GameState& state) {
    ClearBackground(GRAY);

    for (const Slot<Particle>& slot : particle_system.particle_pool.data) {
        if (!slot.alive) continue;

        const Particle& particle = slot.ref;

        const Vec2F top_left = particle.position - Vec2F{.x = particle.size / 2, .y = particle.size / 2};

        DrawRectangle((int)top_left.x, (int)top_left.y, (int)particle.size, (int)particle.size, particle.color);
    }

    ui.begin_ui();

    ui.begin_layout("ctr_particle_vars", {.direction = UI::LayoutDirection::Vertical});

    ui.text("txt_particle_template", "Particle config", {.font_size = 20});

    ui_float_variable("speed start min", PARTICLE_TEMPLATE.speed.start.min);
    ui_float_variable("speed start max", PARTICLE_TEMPLATE.speed.start.max);
    ui_float_variable("speed end min", PARTICLE_TEMPLATE.speed.end.min);
    ui_float_variable("speed end max", PARTICLE_TEMPLATE.speed.end.max);

    ui_float_variable("size start min", PARTICLE_TEMPLATE.size.start.min);
    ui_float_variable("size start max", PARTICLE_TEMPLATE.size.start.max);
    ui_float_variable("size end min", PARTICLE_TEMPLATE.size.end.min);
    ui_float_variable("size end max", PARTICLE_TEMPLATE.size.end.max);

    ui_color_variable("color start", PARTICLE_TEMPLATE.color.start);
    ui_color_variable("color end", PARTICLE_TEMPLATE.color.end);

    ui_float_variable("lifetime min", PARTICLE_TEMPLATE.lifetime.min);
    ui_float_variable("lifetime max", PARTICLE_TEMPLATE.lifetime.max);

    ui_float_variable("gravity x", PARTICLE_TEMPLATE.gravity.x);
    ui_float_variable("gravity y", PARTICLE_TEMPLATE.gravity.y);

    ui.end_layout();

    ui.end_ui();
}
} // namespace

const Scene PARTICLE_SCENE = {.name = "Particles", .init = nullptr, .update = Update, .draw = Draw, .destroy = nullptr};
