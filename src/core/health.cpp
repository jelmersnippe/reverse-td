#include "core/health.hpp"
#include "core/renderer.hpp"

const Vec2F HEALTH_BAR_SIZE = {.x = 50, .y = 10};

void DrawHealth(const Vec2F& position, const Health& health) {
    if (health.current >= health.max) return;

    Vec2F top_left = position - HEALTH_BAR_SIZE * 0.5f;

    render_rectangle(top_left, HEALTH_BAR_SIZE, RED, false);
    render_rectangle(top_left,
                     HEALTH_BAR_SIZE * Vec2F{.x = ((float)health.animated_health / (float)health.max), .y = 1}, GREEN,
                     false);
    render_rectangle(top_left, HEALTH_BAR_SIZE, BLACK, false, true);
}
