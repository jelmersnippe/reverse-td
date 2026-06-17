#include "core/health.hpp"
#include "core/renderer.hpp"

void DrawHealth(const Vec2F& position, const Health& health) {
    if (health.current >= health.max) return;

    Vec2F top_left = position - Vec2F{.x = 25, .y = 5};

    render_rectangle(top_left, {.x = 50, .y = 10}, RED, false);
    render_rectangle(top_left, {.x = 50 * ((float)health.animated_health / (float)health.max), .y = 10}, GREEN, false);
}
