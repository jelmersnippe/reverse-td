#pragma once

#include "core/data.hpp"
#include "key_maps.hpp"
#include "raylib.h"
#include <array>
#include <cstddef>

struct InputState {
    Vec2F mouse_position = {};

    std::array<bool, (size_t)Key::COUNT> key_down;
    std::array<bool, (size_t)Key::COUNT> key_pressed;
    std::array<bool, (size_t)Key::COUNT> key_released;

    std::array<bool, (size_t)Mouse::COUNT> mouse_down;
    std::array<bool, (size_t)Mouse::COUNT> mouse_pressed;
    std::array<bool, (size_t)Mouse::COUNT> mouse_released;
};

struct InputFrame {
    InputState state;

    std::array<bool, (size_t)Key::COUNT> consumed_keys;
    std::array<bool, 3> consumed_mouse;

    bool is_key_down(Key key) const;
    bool is_key_pressed(Key key) const;
    bool is_key_released(Key key) const;

    bool is_mouse_down(Mouse button) const;
    bool is_mouse_pressed(Mouse button) const;
    bool is_mouse_released(Mouse button) const;

    void consume_key(Key key);
    void consume_mouse(Mouse button);

    void update();
};

inline InputFrame input_frame = {};

inline bool InputFrame::is_key_down(Key key) const {
    return this->state.key_down[(size_t)key] && !this->consumed_keys[(size_t)key];
}
inline bool InputFrame::is_key_pressed(Key key) const {
    return this->state.key_pressed[(size_t)key] && !this->consumed_keys[(size_t)key];
}
inline bool InputFrame::is_key_released(Key key) const {
    return this->state.key_released[(size_t)key] && !this->consumed_keys[(size_t)key];
}

inline bool InputFrame::is_mouse_down(Mouse button) const {
    return this->state.mouse_down[(size_t)button] && !this->consumed_mouse[(size_t)button];
}
inline bool InputFrame::is_mouse_pressed(Mouse button) const {
    return this->state.mouse_pressed[(size_t)button] && !this->consumed_mouse[(size_t)button];
}
inline bool InputFrame::is_mouse_released(Mouse button) const {
    return this->state.mouse_released[(size_t)button] && !this->consumed_mouse[(size_t)button];
}

inline void InputFrame::consume_key(Key key) {
    this->consumed_keys[(size_t)key] = true;
}
inline void InputFrame::consume_mouse(Mouse button) {
    this->consumed_mouse[(size_t)button] = true;
}

inline void InputFrame::update() {
    Vector2 mouse_pos = GetMousePosition();
    this->state.mouse_position = Vec2F{.x = mouse_pos.x, .y = mouse_pos.y};
    for (const KeyboardKey raylibKey : KeyMaps::key_to_raylib_keyboardkey) {
        Key key = KeyMaps::raylib_keyboardkey_to_key[raylibKey];

        if (key == Key::Unknown) continue;

        this->state.key_down[(size_t)key] = IsKeyDown(raylibKey);
        this->state.key_pressed[(size_t)key] = IsKeyPressed(raylibKey);
        this->state.key_released[(size_t)key] = IsKeyReleased(raylibKey);
    }

    for (const MouseButton raylibMouseButton : KeyMaps::mouse_to_raylib_mouse_button) {
        Mouse mouse = KeyMaps::raylib_mousebutton_to_mouse[raylibMouseButton];

        this->state.mouse_down[(size_t)mouse] = IsMouseButtonDown(raylibMouseButton);
        this->state.mouse_pressed[(size_t)mouse] = IsMouseButtonPressed(raylibMouseButton);
        this->state.mouse_released[(size_t)mouse] = IsMouseButtonReleased(raylibMouseButton);
    }

    this->consumed_keys.fill(false);
    this->consumed_mouse.fill(false);
}
