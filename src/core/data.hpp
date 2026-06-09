#pragma once

struct Vec2 {
    int x = 0;
    int y = 0;

    Vec2 operator+(const Vec2& vec) const { return Vec2{.x = x + vec.x, .y = y + vec.y}; }
    Vec2 operator*(const Vec2& vec) const { return Vec2{.x = x * vec.x, .y = y * vec.y}; }
    Vec2 operator-(const Vec2& vec) const { return Vec2{.x = x - vec.x, .y = y - vec.y}; }

    void operator+=(const Vec2& vec) {
        x = x + vec.x;
        y = y + vec.y;
    }
    void operator*=(const Vec2& vec) {
        x = x * vec.x;
        y = y * vec.y;
    }
    void operator-=(const Vec2& vec) {
        x = x - vec.x;
        y = y - vec.y;
    }

    Vec2 operator*(const int& value) const { return Vec2{.x = x * value, .y = y * value}; }
    Vec2 operator/(const int& value) { return Vec2{.x = x / value, .y = y / value}; }
};

struct Vec2F {
    float x = 0;
    float y = 0;

    Vec2F operator+(const Vec2F& vec) const { return Vec2F{.x = x + vec.x, .y = y + vec.y}; }
    Vec2F operator*(const Vec2F& vec) const { return Vec2F{.x = x * vec.x, .y = y * vec.y}; }
    Vec2F operator-(const Vec2F& vec) const { return Vec2F{.x = x - vec.x, .y = y - vec.y}; }

    void operator+=(const Vec2F& vec) {
        x = x + vec.x;
        y = y + vec.y;
    }
    void operator*=(const Vec2F& vec) {
        x = x * vec.x;
        y = y * vec.y;
    }
    void operator-=(const Vec2F& vec) {
        x = x - vec.x;
        y = y - vec.y;
    }

    Vec2F operator*(const float& value) const { return Vec2F{.x = x * value, .y = y * value}; }
    Vec2F operator/(const float& value) const { return Vec2F{.x = x / value, .y = y / value}; }
};

struct Rect {
    Vec2 position;
    Vec2 size;
};
