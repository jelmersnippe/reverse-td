#pragma once

#include "raylib.h"
#include <algorithm>
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

struct Vec2F;

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
    void operator*=(const int& value) {
        x = x * value;
        y = y * value;
    }
    void operator+=(const int& value) {
        x = x + value;
        y = y + value;
    }
    void operator-=(const int& value) {
        x = x - value;
        y = y - value;
    }

    Vec2 operator*(const int& value) const { return Vec2{.x = x * value, .y = y * value}; }
    Vec2 operator/(const int& value) { return Vec2{.x = x / value, .y = y / value}; }

    Vec2 operator*(const float& value) const {
        return Vec2{.x = (int)((float)x * value), .y = (int)((float)y * value)};
    }
    Vec2 operator/(const float& value) const {
        return Vec2{.x = (int)((float)x / value), .y = (int)((float)y / value)};
    }

    int length() const { return std::sqrt(x * x + y * y); }

    Vec2 normalized() const {
        int len = length();
        if (len == 0) { return Vec2{}; }

        return {.x = x / len, .y = y / len};
    }

    float distance(Vec2 other) const {
        Vec2 dif = other - *this;
        return std::sqrt(dif.x * dif.x + dif.y * dif.y);
    }

    Vector2 to_raylib() const { return Vector2{.x = (float)this->x, .y = (float)this->y}; }

    static Vec2 from_raylib(const Vector2& v);
};

inline Vec2 Vec2::from_raylib(const Vector2& v) {
    return Vec2{.x = (int)v.x, .y = (int)v.y};
}

struct Vec2F {
    float x = 0;
    float y = 0;

    Vec2F operator+(const Vec2F& vec) const { return Vec2F{.x = x + vec.x, .y = y + vec.y}; }
    Vec2F operator*(const Vec2F& vec) const { return Vec2F{.x = x * vec.x, .y = y * vec.y}; }
    Vec2F operator/(const Vec2F& vec) const { return Vec2F{.x = x / vec.x, .y = y / vec.y}; }
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

    void operator*=(const float& value) {
        x = x * value;
        y = y * value;
    }
    void operator+=(const float& value) {
        x = x + value;
        y = y + value;
    }
    void operator-=(const float& value) {
        x = x - value;
        y = y - value;
    }

    Vec2F operator*(const float& value) const { return Vec2F{.x = x * value, .y = y * value}; }
    Vec2F operator/(const float& value) const { return Vec2F{.x = x / value, .y = y / value}; }

    float length() const { return std::sqrt(x * x + y * y); }

    Vec2F normalized() const {
        float len = length();
        if (len == 0.0f) { return Vec2F{}; }

        return {.x = x / len, .y = y / len};
    }

    float angle_to(Vec2F other) const {
        Vec2F direction = other - *this;
        return atan2f(direction.y, direction.x) * RAD2DEG;
    }

    float angle() const { return atan2f(this->y, this->x) * RAD2DEG; }

    Vec2F rotate(float angle) const {
        const float rad = angle * DEG2RAD;
        return {.x = cosf(rad) * this->x - sinf(rad) * this->y, .y = sinf(rad) * this->x + cosf(rad) * this->y};
    }

    float distance_to(Vec2F other) const {
        Vec2F dif = other - *this;
        return std::sqrt(dif.x * dif.x + dif.y * dif.y);
    }

    Vec2F direction_to(Vec2F position) const { return position - *this; }

    Vector2 to_raylib() const { return Vector2{.x = this->x, .y = this->y}; }

    void clamp(Vec2F min, Vec2F max) {
        this->x = std::clamp(this->x, min.x, max.y);
        this->y = std::clamp(this->y, min.x, max.y);
    }

    static Vec2F from_raylib(const Vector2& v) { return Vec2F{.x = v.x, .y = v.y}; }

    static Vec2F from_vec2(const Vec2& v) { return Vec2F{.x = static_cast<float>(v.x), .y = static_cast<float>(v.y)}; }

    static Vec2F from_angle(float angle) {
        float rad = angle * DEG2RAD;
        return Vec2F{.x = cosf(rad), .y = sinf(rad)};
    }
};

struct Rect {
    Vec2F position;
    Vec2F size;

    Rect operator*(const float& value) const { return Rect{.position = position * value, .size = size * value}; }
};
