#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

enum class Mouse : uint16_t {
    Left,
    Middle,
    Right,
    COUNT
};

enum class Key : uint16_t {
    // Letters
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    // Numbers (top row)
    Num0,
    Num1,
    Num2,
    Num3,
    Num4,
    Num5,
    Num6,
    Num7,
    Num8,
    Num9,

    // Function keys
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,

    // Arrows
    Left,
    Right,
    Up,
    Down,

    // Controls
    Escape,
    Enter,
    Tab,
    Space,
    Backspace,
    Insert,
    Delete,
    Home,
    End,
    PageUp,
    PageDown,

    // Modifiers
    LeftShift,
    RightShift,
    LeftCtrl,
    RightCtrl,
    LeftAlt,
    RightAlt,

    // Numpad
    KP0,
    KP1,
    KP2,
    KP3,
    KP4,
    KP5,
    KP6,
    KP7,
    KP8,
    KP9,
    KPDecimal,
    KPDivide,
    KPMultiply,
    KPSubtract,
    KPAdd,
    KPEnter,

    Unknown,

    COUNT
};

inline constexpr std::array<const char*, (std::size_t)Key::COUNT> KeyNames = {
    "A",         "B",          "C",        "D",         "E",         "F",        "G",      "H",    "I",   "J",
    "K",         "L",          "M",        "N",         "O",         "P",        "Q",      "R",    "S",   "T",
    "U",         "V",          "W",        "X",         "Y",         "Z",

    "0",         "1",          "2",        "3",         "4",         "5",        "6",      "7",    "8",   "9",

    "F1",        "F2",         "F3",       "F4",        "F5",        "F6",       "F7",     "F8",   "F9",  "F10",
    "F11",       "F12",

    "Left",      "Right",      "Up",       "Down",

    "Escape",    "Enter",      "Tab",      "Space",     "Backspace", "Insert",   "Delete", "Home", "End", "PageUp",
    "PageDown",

    "LeftShift", "RightShift", "LeftCtrl", "RightCtrl", "LeftAlt",   "RightAlt",

    "KP0",       "KP1",        "KP2",      "KP3",       "KP4",       "KP5",      "KP6",    "KP7",  "KP8", "KP9",

    "KP.",       "KP/",        "KP*",      "KP-",       "KP+",       "KPEnter",

    "Unknown"};
inline constexpr std::array<const char*, (std::size_t)Mouse::COUNT> MouseNames = {"Left", "Middle", "Right"};

inline const char* key_to_string(Key k) {
    auto i = (size_t)k;
    assert(i < KeyNames.size() && "key_to_string key out of range");

    return KeyNames[i];
}

inline const char* mouse_to_string(Mouse m) {
    auto i = (size_t)m;
    assert(i < MouseNames.size() && "mouse_to_string mouse out of range");

    return MouseNames[i];
}
