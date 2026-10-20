#include "waveform_renderer.hpp"
#include "ui_helpers.hpp"

#include <cstddef>

namespace encodeco {
namespace {
void drawDashedVLine(float x, float y1, float y2, Color color) {
    constexpr float dash = 4.0f;
    constexpr float gapLen = 4.0f;
    for (float y = y1; y < y2; y += dash + gapLen) {
        const float yEnd = (y + dash < y2) ? y + dash : y2;
        DrawLineEx({x, y}, {x, yEnd}, 1.0f, color);
    }
}
} // namespace

void drawWaveform(const AppState& state) {
    const Rectangle plot{55, 445, 990, 355};
    DrawRectangleRounded(plot, 0.025f, 8, PLOT_BG);
    DrawRectangleRoundedLines(plot, 0.025f, 8, GRID);

    const float left = plot.x + 35;
    const float right = plot.x + plot.width - 20;
    const float top = plot.y + 45;
    const float bottom = plot.y + plot.height - 45;
    const float middle = (top + bottom) / 2.0f;
    const float amplitude = (bottom - top) * 0.25f;

    if (state.bits.empty() || state.levels.empty()) {
        drawText("No signal to display", left, middle, 18, MUTED);
        return;
    }

    const float bitWidth = (right - left) / static_cast<float>(state.bits.size());
    const float sampleWidth = (right - left) / static_cast<float>(state.levels.size());
    const bool labelEveryTick = sampleWidth >= 22.0f;

    for (int i = 0; i <= 4; ++i) {
        const float y = top + (bottom - top) * i / 4.0f;
        DrawLineEx({left, y}, {right, y}, 1.0f, GRID);
    }

    for (std::size_t i = 0; i <= state.levels.size(); ++i) {
        const float x = left + static_cast<float>(i) * sampleWidth;
        const bool onBitBoundary = (i * state.bits.size()) % state.levels.size() == 0;

        if (onBitBoundary) {
            DrawLineEx({x, top}, {x, bottom}, 1.5f, GRID_STRONG);
            DrawLineEx({x, bottom}, {x, bottom + 9}, 1.5f, MUTED);
        } else {
            drawDashedVLine(x, top, bottom, GRID_HALF);
            DrawLineEx({x, bottom}, {x, bottom + 5}, 1.0f, MUTED);
        }

        if (onBitBoundary || labelEveryTick) {
            const char* tickLabel = TextFormat("%d", static_cast<int>(i));
            const float labelSize = 11.0f;
            const float w = textWidth(tickLabel, labelSize);
            drawText(tickLabel, x - w / 2.0f, bottom + 11, labelSize,
                     onBitBoundary ? TEXT_PRIMARY : MUTED);
        }
    }

    for (std::size_t i = 0; i < state.bits.size(); ++i) {
        const float x = left + static_cast<float>(i) * bitWidth;
        const char* bitLabel = state.bits[i] == 0 ? "0" : "1";
        const float labelWidth = textWidth(bitLabel, 16);
        drawText(bitLabel, x + (bitWidth - labelWidth) / 2.0f,
                 plot.y + 12, 16, TEXT_PRIMARY);
    }

    auto levelY = [&](int level) -> float {
        return middle - static_cast<float>(level) * amplitude;
    };

    for (std::size_t i = 0; i < state.levels.size(); ++i) {
        const float x1 = left + static_cast<float>(i) * sampleWidth;
        const float x2 = x1 + sampleWidth;
        const float y = levelY(state.levels[i]);
        DrawLineEx({x1, y}, {x2, y}, 3.0f, SIGNAL);
        if (i + 1 < state.levels.size() && state.levels[i] != state.levels[i + 1]) {
            DrawLineEx({x2, y}, {x2, levelY(state.levels[i + 1])}, 3.0f, SIGNAL);
        }
    }

    drawText("+1", 15, middle - amplitude - 8, 16, MUTED);
    drawText("0", 25, middle - 8, 16, MUTED);
    drawText("-1", 15, middle + amplitude - 8, 16, MUTED);
    drawText("Time axis: solid = bit boundary, dashed = half-bit (2 time units per bit)",
             left, bottom + 28, 13, MUTED);
}

} // namespace encodeco
