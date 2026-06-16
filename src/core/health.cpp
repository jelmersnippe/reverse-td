#include "core/health.hpp"
#include "core/renderer.hpp"
#include <format>

void DrawHealth(const Vec2F& position, const Health& health) {
    if (health.current >= health.max) return;

    const std::string health_text = std::format("{}/{}", health.current, health.max);

    Vec2F top_left = position - Vec2F{.x = 10, .y = 2.5};

    render_rectangle(top_left, {.x = 20, .y = 5}, RED, false);
    render_rectangle(top_left, {.x = 20 * ((float)health.current / (float)health.max), .y = 5}, GREEN, false);

    render_text(health_text, position, 12, BLACK);
}
