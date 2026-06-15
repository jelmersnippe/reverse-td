#pragma once

#include "core/data.hpp"
#include "systems/targeting.hpp"
#include <optional>

struct Pickup {
    std::optional<Targetable> target = std::nullopt;
    Vec2F position;
    int value;
};
