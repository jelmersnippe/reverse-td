#include "scenes/particle_scene.hpp"

#include "core/data.hpp"
#include "core/gui.hpp"
#include "core/particles.hpp"
#include "format"
#include "raylib.h"

namespace {

ParticleSystem particle_system;

ParticleTemplate PARTICLE_TEMPLATE =
    ParticleTemplate({.speed = {.start = {.min = 40, .max = 120}, .end = {.min = 10, .max = 30}},
                      .size = {.start = {.min = 8, .max = 14}, .end = {.min = 0, .max = 4}},
                      .color = {.start = Color(255, 180, 50, 255), .end = Color(40, 40, 40, 0)},
                      .lifetime = {.min = 0.4f, .max = 1.0f},
                      .gravity = {.x = 0, .y = -20}});
Emitter EMITTER = Emitter{.position = {.x = 0, .y = 0},
                          .direction = {.x = 0, .y = -1},
                          .spread = 0.6f,
                          .particle_template = PARTICLE_TEMPLATE,
                          .rate = 60,
                          .duration = 10};

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

bool PARTICLES_COLLAPSED = false;
bool EMITTER_COLLAPSED = false;

std::unordered_map<std::string, bool> is_collapsed_states = {};

void ui_int_variable(std::string label, int& variable) {
    const auto is_collapsed_it = is_collapsed_states.find(label);
    bool is_collapsed = true;
    if (is_collapsed_it != is_collapsed_states.end()) is_collapsed = is_collapsed_it->second;

    ui.begin_layout("variable_" + label, {.direction = UI::LayoutDirection::Vertical,
                                          .padding = 4,
                                          .gap = 8,
                                          .width = INPUT_WIDTH,
                                          .color = {.border = BLACK}});

    // Label
    if (ui.begin_button("btn_collapse_" + label, COLLAPSIBLE_BUTTON_STYLE)) is_collapsed_states[label] = !is_collapsed;
    ui.text("txt_" + label, label, LABEL_TEXT_STYLE);
    std::string caret = "/\\";
    if (is_collapsed) caret = "\\/";
    ui.text("txt_collapse_caret_" + label, caret, {.font_size = 20});
    ui.end_button();

    if (!is_collapsed) {
        // Input
        ui.begin_layout("input_" + label, INPUT_LAYOUT_STYLE);
        ui.text("txt_value_" + label, std::format("{}", variable), BUTTON_TEXT_STYLE);

        // Buttons
        ui.begin_layout("btns_" + label, {.direction = UI::LayoutDirection::Vertical});
        if (ui.begin_button("btn_up_" + label, BUTTON_STYLE, {.hold_enabled = true})) variable += 1;
        ui.text("btn_txt_up_" + label, "+", BUTTON_TEXT_STYLE);
        ui.end_button();

        if (ui.begin_button("btn_down_" + label, BUTTON_STYLE, {.hold_enabled = true})) variable -= 1;
        ui.text("btn_txt_down_" + label, "-", BUTTON_TEXT_STYLE);
        ui.end_button();

        ui.end_layout();

        ui.end_layout();
    }

    ui.end_layout();
}

void ui_float_variable(std::string label, float& variable) {
    const auto is_collapsed_it = is_collapsed_states.find(label);
    bool is_collapsed = true;
    if (is_collapsed_it != is_collapsed_states.end()) is_collapsed = is_collapsed_it->second;

    ui.begin_layout("variable_" + label, {.direction = UI::LayoutDirection::Vertical,
                                          .padding = 4,
                                          .gap = 8,
                                          .width = INPUT_WIDTH,
                                          .color = {.border = BLACK}});

    // Label
    if (ui.begin_button("btn_collapse_" + label, COLLAPSIBLE_BUTTON_STYLE)) is_collapsed_states[label] = !is_collapsed;
    ui.text("txt_" + label, label, LABEL_TEXT_STYLE);
    std::string caret = "/\\";
    if (is_collapsed) caret = "\\/";
    ui.text("txt_collapse_caret_" + label, caret, {.font_size = 20});
    ui.end_button();

    if (!is_collapsed) {
        // Input
        ui.begin_layout("input_" + label, INPUT_LAYOUT_STYLE);
        ui.text("txt_value_" + label, std::format("{}", variable), BUTTON_TEXT_STYLE);

        // Buttons
        ui.begin_layout("btns_" + label, {.direction = UI::LayoutDirection::Vertical});
        if (ui.begin_button("btn_up_" + label, BUTTON_STYLE, {.hold_enabled = true})) variable += 0.1f;
        ui.text("btn_txt_up_" + label, "+", BUTTON_TEXT_STYLE);
        ui.end_button();

        if (ui.begin_button("btn_down_" + label, BUTTON_STYLE, {.hold_enabled = true})) variable -= 0.1f;
        ui.text("btn_txt_down_" + label, "-", BUTTON_TEXT_STYLE);
        ui.end_button();

        ui.end_layout();

        ui.end_layout();
    }

    ui.end_layout();
}

void ui_color_variable(std::string label, Color& variable) {
    ui.begin_layout("variable_" + label, {.direction = UI::LayoutDirection::Vertical,
                                          .padding = 4,
                                          .gap = 8,
                                          .width = INPUT_WIDTH,
                                          .color = {.border = BLACK}});

    ui.text("txt_" + label, label, LABEL_TEXT_STYLE);

    ui.color_picker("cp_" + label, variable);
    ui.text("text_" + label + "_start",
            std::format("RGBA ({},{},{},{})", variable.r, variable.g, variable.b, variable.a), BUTTON_TEXT_STYLE);

    ui.end_layout();
}

void Draw(GameState& state) {
    ClearBackground(GRAY);

    for (auto& slot : particle_system.particle_pool.data) {
        if (!slot.alive) continue;

        const Particle& particle = slot.ref;

        const Vec2F top_left = particle.position - Vec2F{.x = particle.size / 2, .y = particle.size / 2};

        DrawRectangle((int)top_left.x, (int)top_left.y, (int)particle.size, (int)particle.size, particle.color);
    }

    ui.begin_ui();

    ui.begin_layout(
        "ctr_particle_vars",
        {.direction = UI::LayoutDirection::Vertical, .padding = 8, .color = {.border = BLACK, .background = WHITE}});

    if (ui.begin_button("btn_emitter_collapse", COLLAPSIBLE_BUTTON_STYLE)) EMITTER_COLLAPSED = !EMITTER_COLLAPSED;
    ui.text("txt_emitter_template", "Emitter config", {.font_size = 20});
    std::string emitter_caret = "/\\";
    if (EMITTER_COLLAPSED) emitter_caret = "\\/";
    ui.text("txt_emitter_template_caret", emitter_caret, {.font_size = 20});
    ui.end_button();

    if (!EMITTER_COLLAPSED) {
        // Type picker
        ui.begin_layout("select_emitter_type",
                        {.direction = UI::LayoutDirection::Horizontal, .align_items = UI::AlignItems::CENTER});
        if (ui.begin_button("option_point", {})) EMITTER.type = EmitterType::Point;
        ui.text("option_txt_point", "Point", {});
        ui.end_button();

        if (ui.begin_button("option_circle", {})) EMITTER.type = EmitterType::Circle;
        ui.text("option_txt_circle", "Circle", {});
        ui.end_button();

        if (ui.begin_button("option_box", {})) EMITTER.type = EmitterType::Box;
        ui.text("option_txt_box", "Box", {});
        ui.end_button();

        ui.end_layout();

        // Type param
        if (EMITTER.type == EmitterType::Circle) {
            ui_float_variable("radius", EMITTER.radius);
        } else if (EMITTER.type == EmitterType::Box) {
            ui_float_variable("box size x", EMITTER.box_size.x);
            ui_float_variable("box size y", EMITTER.box_size.y);
        }

        ui_float_variable("direction x", EMITTER.direction.x);
        ui_float_variable("direction y", EMITTER.direction.y);

        ui_float_variable("spread", EMITTER.spread);

        ui_float_variable("rate", EMITTER.rate);
        ui_int_variable("burst", EMITTER.burst);
    }

    if (ui.begin_button("btn_particle_collapse", COLLAPSIBLE_BUTTON_STYLE)) PARTICLES_COLLAPSED = !PARTICLES_COLLAPSED;
    ui.text("txt_particle_template", "Particle config", {.font_size = 20});
    std::string particle_caret = "/\\";
    if (PARTICLES_COLLAPSED) particle_caret = "\\/";
    ui.text("txt_particle_template_caret", particle_caret, {.font_size = 20});
    ui.end_button();

    if (!PARTICLES_COLLAPSED) {
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
    }

    ui.end_layout();

    ui.end_ui();
}
} // namespace

const Scene PARTICLE_SCENE = {.name = "Particles", .init = nullptr, .update = Update, .draw = Draw, .destroy = nullptr};
