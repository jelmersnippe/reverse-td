#include "scenes/particle_scene.hpp"

#include "core/data.hpp"
#include "core/gui.hpp"
#include "core/input.hpp"
#include "core/particles.hpp"
#include "format"
#include "raylib.h"
#include <climits>

namespace {

UI ui = UI(Vec2F{.x = 25, .y = 25});

bool PARTICLES_COLLAPSED = false;
bool EMITTER_COLLAPSED = false;

std::unordered_map<std::string, bool> is_collapsed_states = {};

template <typename T> std::string format_number(T value, int precision) {
    if constexpr (std::floating_point<T>) { return std::format("{:.{}f}", value, precision); }

    return std::format("{}", value);
};
template <typename T> void ui_number_variable(std::string label, T& variable, Range<T> range, T step = 1) {
    const auto is_collapsed_it = is_collapsed_states.find(label);
    bool is_collapsed = true;
    if (is_collapsed_it != is_collapsed_states.end()) is_collapsed = is_collapsed_it->second;

    ui.begin_layout("variable_" + label,
                    {.direction = UI::LayoutDirection::Vertical, .padding = 4, .gap = 8, .color = {.border = BLACK}});

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
        ui.text("txt_value_" + label, format_number(variable, 2), BUTTON_TEXT_STYLE);

        // Buttons
        ui.begin_layout("btns_" + label, {.direction = UI::LayoutDirection::Vertical});
        if (ui.begin_button("btn_up_" + label, BUTTON_STYLE, {.hold_enabled = true}))
            variable = std::clamp(variable + step, range.min, range.max);
        ui.text("btn_txt_up_" + label, "+", BUTTON_TEXT_STYLE);
        ui.end_button();

        if (ui.begin_button("btn_down_" + label, BUTTON_STYLE, {.hold_enabled = true}))
            variable = std::clamp(variable - step, range.min, range.max);
        ui.text("btn_txt_down_" + label, "-", BUTTON_TEXT_STYLE);
        ui.end_button();

        ui.end_layout();

        ui.end_layout();
    }

    ui.end_layout();
}

void ui_color_variable(std::string label, Color& variable) {
    ui.begin_layout("variable_" + label,
                    {.direction = UI::LayoutDirection::Vertical, .padding = 4, .gap = 8, .color = {.border = BLACK}});

    ui.text("txt_" + label, label, LABEL_TEXT_STYLE);

    ui.color_picker("cp_" + label, variable);
    ui.text("text_" + label + "_start",
            std::format("RGBA ({},{},{},{})", variable.r, variable.g, variable.b, variable.a), BUTTON_TEXT_STYLE);

    ui.end_layout();
}

ParticleSystem particle_system;

ParticleTemplate PARTICLE_TEMPLATE = ParticleTemplate({
    .speed = {.start = {.min = 150, .max = 300}, .end = {.min = 50, .max = 100}},
    .size = {.start = {.min = 6, .max = 14}, .end = {.min = 0, .max = 2}},
    .color = {.start = Color(255, 255, 180, 255), .end = Color(255, 100, 20, 0)},
    .lifetime = {.min = 0.03f, .max = 0.08f},
});
Emitter EMITTER = Emitter{.position = {.x = 0, .y = 0},
                          .direction = {.x = 0, .y = -1},
                          .spread = 20,
                          .particle_template = PARTICLE_TEMPLATE,
                          .rate = 0,
                          .duration = 0,
                          .burst = 50};

void update_ui() {
    ui.begin_ui();

    ui.begin_layout("ctr_config", {.direction = UI::LayoutDirection::Vertical,
                                   .padding = 8,
                                   .gap = 8,
                                   .width = INPUT_WIDTH,
                                   .color = {.border = BLACK, .background = WHITE}});

    ui.begin_layout("ctr_emitter_vars",
                    {.direction = UI::LayoutDirection::Vertical, .color = {.border = BLACK, .background = WHITE}});

    if (ui.begin_button("btn_emitter_collapse", COLLAPSIBLE_BUTTON_STYLE)) EMITTER_COLLAPSED = !EMITTER_COLLAPSED;
    ui.text("txt_emitter_template", "Emitter config", {.font_size = 20});
    std::string emitter_caret = "/\\";
    if (EMITTER_COLLAPSED) emitter_caret = "\\/";
    ui.text("txt_emitter_template_caret", emitter_caret, {.font_size = 20});
    ui.end_button();

    if (!EMITTER_COLLAPSED) {
        // Type picker
        ui.begin_layout("select_emitter_type", {.direction = UI::LayoutDirection::Horizontal,
                                                .justify_content = UI::JustifyContent::SPACE_BETWEEN,
                                                .align_items = UI::AlignItems::CENTER});
        auto point_style = OPTION_STYLE;
        if (EMITTER.type == EmitterType::Point) point_style.color.background = GRAY;
        if (ui.begin_button("option_point", point_style)) EMITTER.type = EmitterType::Point;
        ui.text("option_txt_point", "Point", {});
        ui.end_button();

        auto circle_style = OPTION_STYLE;
        if (EMITTER.type == EmitterType::Circle) circle_style.color.background = GRAY;
        if (ui.begin_button("option_circle", circle_style)) EMITTER.type = EmitterType::Circle;
        ui.text("option_txt_circle", "Circle", {});
        ui.end_button();

        auto box_style = OPTION_STYLE;
        if (EMITTER.type == EmitterType::Box) box_style.color.background = GRAY;
        if (ui.begin_button("option_box", box_style)) EMITTER.type = EmitterType::Box;
        ui.text("option_txt_box", "Box", {});
        ui.end_button();

        ui.end_layout();

        // Type param
        if (EMITTER.type == EmitterType::Circle) {
            ui_number_variable("radius", EMITTER.radius, {.min = 0, .max = 360}, 0.1f);
        } else if (EMITTER.type == EmitterType::Box) {
            ui_number_variable("box size x", EMITTER.box_size.x, {.min = 0, .max = MAXFLOAT});
            ui_number_variable("box size y", EMITTER.box_size.y, {.min = 0, .max = MAXFLOAT});
        }

        ui_number_variable("direction x", EMITTER.direction.x, {.min = -MAXFLOAT, .max = MAXFLOAT}, 0.1f);
        ui_number_variable("direction y", EMITTER.direction.y, {.min = -MAXFLOAT, .max = MAXFLOAT}, 0.1f);

        ui_number_variable("spread", EMITTER.spread, {.min = 0, .max = 360}, 1.0f);

        ui_number_variable("rate", EMITTER.rate, {.min = 0, .max = MAXFLOAT}, 0.1f);
        ui_number_variable("burst", EMITTER.burst, {.min = 0, .max = INT_MAX});
    }

    ui.end_layout();

    ui.begin_layout("ctr_particle_vars",
                    {.direction = UI::LayoutDirection::Vertical, .color = {.border = BLACK, .background = WHITE}});

    if (ui.begin_button("btn_particle_collapse", COLLAPSIBLE_BUTTON_STYLE)) PARTICLES_COLLAPSED = !PARTICLES_COLLAPSED;
    ui.text("txt_particle_template", "Particle config", {.font_size = 20});
    std::string particle_caret = "/\\";
    if (PARTICLES_COLLAPSED) particle_caret = "\\/";
    ui.text("txt_particle_template_caret", particle_caret, {.font_size = 20});
    ui.end_button();

    if (!PARTICLES_COLLAPSED) {
        ui_number_variable("speed start min", PARTICLE_TEMPLATE.speed.start.min,
                           {.min = 0, .max = PARTICLE_TEMPLATE.speed.start.max});
        ui_number_variable("speed start max", PARTICLE_TEMPLATE.speed.start.max,
                           {.min = PARTICLE_TEMPLATE.speed.start.min, .max = MAXFLOAT});
        ui_number_variable("speed end min", PARTICLE_TEMPLATE.speed.end.min,
                           {.min = 0, .max = PARTICLE_TEMPLATE.speed.end.max});
        ui_number_variable("speed end max", PARTICLE_TEMPLATE.speed.end.max,
                           {.min = PARTICLE_TEMPLATE.speed.end.min, .max = MAXFLOAT});

        ui_number_variable("size start min", PARTICLE_TEMPLATE.size.start.min,
                           {.min = 0, .max = PARTICLE_TEMPLATE.size.start.max});
        ui_number_variable("size start max", PARTICLE_TEMPLATE.size.start.max,
                           {.min = PARTICLE_TEMPLATE.size.start.min, .max = MAXFLOAT});
        ui_number_variable("size end min", PARTICLE_TEMPLATE.size.end.min,
                           {.min = 0, .max = PARTICLE_TEMPLATE.size.end.max});
        ui_number_variable("size end max", PARTICLE_TEMPLATE.size.end.max,
                           {.min = PARTICLE_TEMPLATE.size.end.min, .max = MAXFLOAT});

        ui_color_variable("color start", PARTICLE_TEMPLATE.color.start);
        ui_color_variable("color end", PARTICLE_TEMPLATE.color.end);

        ui_number_variable("lifetime min", PARTICLE_TEMPLATE.lifetime.min,
                           {.min = 0, .max = PARTICLE_TEMPLATE.lifetime.max}, 0.1f);
        ui_number_variable("lifetime max", PARTICLE_TEMPLATE.lifetime.max,
                           {.min = PARTICLE_TEMPLATE.lifetime.min, .max = MAXFLOAT}, 0.1f);

        ui_number_variable("gravity x", PARTICLE_TEMPLATE.gravity.x, {.min = -MAXFLOAT, .max = MAXFLOAT});
        ui_number_variable("gravity y", PARTICLE_TEMPLATE.gravity.y, {.min = -MAXFLOAT, .max = MAXFLOAT});
    }

    ui.end_layout();

    ui.end_layout();

    ui.end_ui();
}

void Update(GameState&) {
    update_ui();

    if (input_frame.is_mouse_pressed(Mouse::Left)) {
        EMITTER.position = input_frame.state.mouse_position;
        EMITTER.particle_template = PARTICLE_TEMPLATE;
        particle_system.play(EMITTER);
    }

    particle_system.update(GetFrameTime());
}

void Draw(GameState&) {
    ClearBackground(GRAY);

    particle_system.draw();

    ui.draw();
}
} // namespace

const Scene PARTICLE_SCENE = {.name = "Particles", .init = nullptr, .update = Update, .draw = Draw, .destroy = nullptr};
