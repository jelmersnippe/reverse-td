#include "scenes/particle_scene.hpp"

#include "core/data.hpp"
#include "core/gui.hpp"
#include "core/particles.hpp"
#include "format"
#include "raylib.h"

namespace {

ParticleSystem particle_system;

ParticleTemplate PARTICLE_TEMPLATE = ParticleTemplate(Vec2F{.x = 0, .y = -100}, 10, RED, 1.5f);

void Update(GameState& state) {
    const Vector2 mouse_pos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        particle_system.play({.position = {.x = mouse_pos.x, .y = mouse_pos.y},
                              .particle_template = PARTICLE_TEMPLATE,
                              .rate = 4,
                              .duration = 5});
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

    if (ui.begin_button("btn_down_" + label, BUTTON_STYLE, true)) variable -= 1;

    ui.text("btn_txt_down_" + label, "-", TEXT_STYLE);
    ui.end_button();

    ui.text("txt_value_" + label, std::format("{}", variable), TEXT_STYLE);

    if (ui.begin_button("btn_up_" + label, BUTTON_STYLE, true)) variable += 1;
    ui.text("btn_txt_up_" + label, "+", TEXT_STYLE);
    ui.end_button();

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

    ui_float_variable("velocity x", PARTICLE_TEMPLATE.velocity.x);
    ui_float_variable("velocity y", PARTICLE_TEMPLATE.velocity.y);
    ui_float_variable("size", PARTICLE_TEMPLATE.size);
    ui_float_variable("lifetime", PARTICLE_TEMPLATE.lifetime);

    ui.color_picker("color", PARTICLE_TEMPLATE.color);

    ui.end_layout();

    ui.end_ui();
}
} // namespace

const Scene PARTICLE_SCENE = {.name = "Particles", .init = nullptr, .update = Update, .draw = Draw, .destroy = nullptr};
