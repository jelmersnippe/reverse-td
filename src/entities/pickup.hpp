#pragma once

#include "core/transform.hpp"
#include "systems/targeting.hpp"
#include <optional>

struct Pickup {
    Transform2D transform;
    std::optional<Targetable> target = std::nullopt;
    int value;
};
