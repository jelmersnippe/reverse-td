#pragma once

#include "raylib.h"
#include "systems/targeting.hpp"

struct Pickup {
    std::optional<Targetable> target;
    Vector2 position;
    int value;
};
