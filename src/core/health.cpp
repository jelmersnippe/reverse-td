#include "core/health.hpp"
#include "core/renderer.hpp"
#include <format>

void DrawHealth(const Vec2F& position, const Health& health) {
    if (health.current >= health.max) return;

    const std::string health_text = std::format("{}/{}", health.current, health.max);
    const int text_width = MeasureText(health_text.c_str(), 12);

    render_rectangle(position, {.x = 20, .y = 5}, RED);
    render_rectangle(position, {.x = 20 * ((float)health.current / (float)health.max), .y = 5}, RED);

    DrawText(health_text.c_str(), position.x - text_width / 2, position.y, 12, BLACK);
}
