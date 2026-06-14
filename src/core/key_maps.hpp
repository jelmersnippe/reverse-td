#pragma once

#include "key.hpp"
#include "raylib.h"
#include <array>
#include <cstddef>

struct KeyMaps {
    static std::array<KeyboardKey, (size_t)Key::COUNT> key_to_raylib_keyboardkey;
    static std::array<Key, (size_t)KEY_KB_MENU + 1> raylib_keyboardkey_to_key;

    static std::array<MouseButton, (size_t)Mouse::COUNT> mouse_to_raylib_mouse_button;
    static std::array<Mouse, (size_t)3> raylib_mousebutton_to_mouse;

    static void init();
};

inline std::array<KeyboardKey, (size_t)Key::COUNT> KeyMaps::key_to_raylib_keyboardkey{};
inline std::array<Key, (size_t)KEY_KB_MENU + 1> KeyMaps::raylib_keyboardkey_to_key{};

inline std::array<MouseButton, (size_t)Mouse::COUNT> KeyMaps::mouse_to_raylib_mouse_button{};
inline std::array<Mouse, (size_t)3> KeyMaps::raylib_mousebutton_to_mouse{};

inline void KeyMaps::init() {
    // 1. Default everything
    key_to_raylib_keyboardkey.fill(KEY_NULL);
    raylib_keyboardkey_to_key.fill(Key::Unknown);

    // =========================
    // Letters
    // =========================
    key_to_raylib_keyboardkey[(size_t)Key::A] = KEY_A;
    key_to_raylib_keyboardkey[(size_t)Key::B] = KEY_B;
    key_to_raylib_keyboardkey[(size_t)Key::C] = KEY_C;
    key_to_raylib_keyboardkey[(size_t)Key::D] = KEY_D;
    key_to_raylib_keyboardkey[(size_t)Key::E] = KEY_E;
    key_to_raylib_keyboardkey[(size_t)Key::F] = KEY_F;
    key_to_raylib_keyboardkey[(size_t)Key::G] = KEY_G;
    key_to_raylib_keyboardkey[(size_t)Key::H] = KEY_H;
    key_to_raylib_keyboardkey[(size_t)Key::I] = KEY_I;
    key_to_raylib_keyboardkey[(size_t)Key::J] = KEY_J;
    key_to_raylib_keyboardkey[(size_t)Key::K] = KEY_K;
    key_to_raylib_keyboardkey[(size_t)Key::L] = KEY_L;
    key_to_raylib_keyboardkey[(size_t)Key::M] = KEY_M;
    key_to_raylib_keyboardkey[(size_t)Key::N] = KEY_N;
    key_to_raylib_keyboardkey[(size_t)Key::O] = KEY_O;
    key_to_raylib_keyboardkey[(size_t)Key::P] = KEY_P;
    key_to_raylib_keyboardkey[(size_t)Key::Q] = KEY_Q;
    key_to_raylib_keyboardkey[(size_t)Key::R] = KEY_R;
    key_to_raylib_keyboardkey[(size_t)Key::S] = KEY_S;
    key_to_raylib_keyboardkey[(size_t)Key::T] = KEY_T;
    key_to_raylib_keyboardkey[(size_t)Key::U] = KEY_U;
    key_to_raylib_keyboardkey[(size_t)Key::V] = KEY_V;
    key_to_raylib_keyboardkey[(size_t)Key::W] = KEY_W;
    key_to_raylib_keyboardkey[(size_t)Key::X] = KEY_X;
    key_to_raylib_keyboardkey[(size_t)Key::Y] = KEY_Y;
    key_to_raylib_keyboardkey[(size_t)Key::Z] = KEY_Z;

    // =========================
    // Numbers
    // =========================
    key_to_raylib_keyboardkey[(size_t)Key::Num0] = KEY_ZERO;
    key_to_raylib_keyboardkey[(size_t)Key::Num1] = KEY_ONE;
    key_to_raylib_keyboardkey[(size_t)Key::Num2] = KEY_TWO;
    key_to_raylib_keyboardkey[(size_t)Key::Num3] = KEY_THREE;
    key_to_raylib_keyboardkey[(size_t)Key::Num4] = KEY_FOUR;
    key_to_raylib_keyboardkey[(size_t)Key::Num5] = KEY_FIVE;
    key_to_raylib_keyboardkey[(size_t)Key::Num6] = KEY_SIX;
    key_to_raylib_keyboardkey[(size_t)Key::Num7] = KEY_SEVEN;
    key_to_raylib_keyboardkey[(size_t)Key::Num8] = KEY_EIGHT;
    key_to_raylib_keyboardkey[(size_t)Key::Num9] = KEY_NINE;

    // =========================
    // Function keys
    // =========================
    key_to_raylib_keyboardkey[(size_t)Key::F1] = KEY_F1;
    key_to_raylib_keyboardkey[(size_t)Key::F2] = KEY_F2;
    key_to_raylib_keyboardkey[(size_t)Key::F3] = KEY_F3;
    key_to_raylib_keyboardkey[(size_t)Key::F4] = KEY_F4;
    key_to_raylib_keyboardkey[(size_t)Key::F5] = KEY_F5;
    key_to_raylib_keyboardkey[(size_t)Key::F6] = KEY_F6;
    key_to_raylib_keyboardkey[(size_t)Key::F7] = KEY_F7;
    key_to_raylib_keyboardkey[(size_t)Key::F8] = KEY_F8;
    key_to_raylib_keyboardkey[(size_t)Key::F9] = KEY_F9;
    key_to_raylib_keyboardkey[(size_t)Key::F10] = KEY_F10;
    key_to_raylib_keyboardkey[(size_t)Key::F11] = KEY_F11;
    key_to_raylib_keyboardkey[(size_t)Key::F12] = KEY_F12;

    // =========================
    // Arrows
    // =========================
    key_to_raylib_keyboardkey[(size_t)Key::Left] = KEY_LEFT;
    key_to_raylib_keyboardkey[(size_t)Key::Right] = KEY_RIGHT;
    key_to_raylib_keyboardkey[(size_t)Key::Up] = KEY_UP;
    key_to_raylib_keyboardkey[(size_t)Key::Down] = KEY_DOWN;

    // =========================
    // Controls
    // =========================
    key_to_raylib_keyboardkey[(size_t)Key::Escape] = KEY_ESCAPE;
    key_to_raylib_keyboardkey[(size_t)Key::Enter] = KEY_ENTER;
    key_to_raylib_keyboardkey[(size_t)Key::Tab] = KEY_TAB;
    key_to_raylib_keyboardkey[(size_t)Key::Space] = KEY_SPACE;
    key_to_raylib_keyboardkey[(size_t)Key::Backspace] = KEY_BACKSPACE;
    key_to_raylib_keyboardkey[(size_t)Key::Insert] = KEY_INSERT;
    key_to_raylib_keyboardkey[(size_t)Key::Delete] = KEY_DELETE;
    key_to_raylib_keyboardkey[(size_t)Key::Home] = KEY_HOME;
    key_to_raylib_keyboardkey[(size_t)Key::End] = KEY_END;
    key_to_raylib_keyboardkey[(size_t)Key::PageUp] = KEY_PAGE_UP;
    key_to_raylib_keyboardkey[(size_t)Key::PageDown] = KEY_PAGE_DOWN;

    // =========================
    // Modifiers
    // =========================
    key_to_raylib_keyboardkey[(size_t)Key::LeftShift] = KEY_LEFT_SHIFT;
    key_to_raylib_keyboardkey[(size_t)Key::RightShift] = KEY_RIGHT_SHIFT;
    key_to_raylib_keyboardkey[(size_t)Key::LeftCtrl] = KEY_LEFT_CONTROL;
    key_to_raylib_keyboardkey[(size_t)Key::RightCtrl] = KEY_RIGHT_CONTROL;
    key_to_raylib_keyboardkey[(size_t)Key::LeftAlt] = KEY_LEFT_ALT;
    key_to_raylib_keyboardkey[(size_t)Key::RightAlt] = KEY_RIGHT_ALT;

    // =========================
    // Numpad
    // =========================
    key_to_raylib_keyboardkey[(size_t)Key::KP0] = KEY_KP_0;
    key_to_raylib_keyboardkey[(size_t)Key::KP1] = KEY_KP_1;
    key_to_raylib_keyboardkey[(size_t)Key::KP2] = KEY_KP_2;
    key_to_raylib_keyboardkey[(size_t)Key::KP3] = KEY_KP_3;
    key_to_raylib_keyboardkey[(size_t)Key::KP4] = KEY_KP_4;
    key_to_raylib_keyboardkey[(size_t)Key::KP5] = KEY_KP_5;
    key_to_raylib_keyboardkey[(size_t)Key::KP6] = KEY_KP_6;
    key_to_raylib_keyboardkey[(size_t)Key::KP7] = KEY_KP_7;
    key_to_raylib_keyboardkey[(size_t)Key::KP8] = KEY_KP_8;
    key_to_raylib_keyboardkey[(size_t)Key::KP9] = KEY_KP_9;

    key_to_raylib_keyboardkey[(size_t)Key::KPDecimal] = KEY_KP_DECIMAL;
    key_to_raylib_keyboardkey[(size_t)Key::KPDivide] = KEY_KP_DIVIDE;
    key_to_raylib_keyboardkey[(size_t)Key::KPMultiply] = KEY_KP_MULTIPLY;
    key_to_raylib_keyboardkey[(size_t)Key::KPSubtract] = KEY_KP_SUBTRACT;
    key_to_raylib_keyboardkey[(size_t)Key::KPAdd] = KEY_KP_ADD;
    key_to_raylib_keyboardkey[(size_t)Key::KPEnter] = KEY_KP_ENTER;

    mouse_to_raylib_mouse_button[(size_t)Mouse::Left] = MOUSE_BUTTON_LEFT;
    mouse_to_raylib_mouse_button[(size_t)Mouse::Middle] = MOUSE_BUTTON_MIDDLE;
    mouse_to_raylib_mouse_button[(size_t)Mouse::Right] = MOUSE_BUTTON_RIGHT;

    // =========================
    // Build reverse maps
    // =========================
    for (size_t i = 0; i < key_to_raylib_keyboardkey.size(); i++) {
        KeyboardKey rk = key_to_raylib_keyboardkey[i];
        if (rk != KEY_NULL) { raylib_keyboardkey_to_key[(int)rk] = (Key)i; }
    }

    for (size_t i = 0; i < mouse_to_raylib_mouse_button.size(); i++) {
        MouseButton rk = mouse_to_raylib_mouse_button[i];
        raylib_mousebutton_to_mouse[(int)rk] = (Mouse)i;
    }
}
