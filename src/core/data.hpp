#pragma once

#include <cmath>

#ifndef PI
#define PI 3.14159265358979323846f
#endif
#ifndef DEG2RAD
#define DEG2RAD (PI / 180.0f)
#endif
#ifndef RAD2DEG
#define RAD2DEG (180.0f / PI)
#endif

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

    Vec2 operator*(const float& value) const {
        return Vec2{.x = (int)((float)x * value), .y = (int)((float)y * value)};
    }
    Vec2 operator/(const float& value) const {
        return Vec2{.x = (int)((float)x / value), .y = (int)((float)y / value)};
    }

    int length() { return std::sqrt(x * x + y * y); }

    Vec2 normalized() {
        int len = length();
        if (len == 0) { return Vec2{}; }

        return {.x = x / len, .y = y / len};
    }
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

    float length() { return std::sqrt(x * x + y * y); }

    Vec2F normalized() {
        float len = length();
        if (len == 0.0f) { return Vec2F{}; }

        return {.x = x / len, .y = y / len};
    }

    float angle_to(Vec2F position) {
        Vec2F direction = position - *this;
        return atan2f(direction.y, direction.x) * RAD2DEG;
    }
};

struct Rect {
    Vec2 position;
    Vec2 size;
};
