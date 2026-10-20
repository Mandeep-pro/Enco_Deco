#include "delta_modulation_panel.hpp"

#include "analog_signal_generator.hpp"
#include "delta_modulation.hpp"
#include "ui_helpers.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <string>
#include <vector>

namespace encodeco {
namespace {

constexpr double pi = 3.14159265358979323846;
constexpr int MAX_SAMPLES = 32;

constexpr double DEFAULT_AMPLITUDE = 1.0;
constexpr double DEFAULT_FREQUENCY = 1.0;
constexpr double DEFAULT_SAMPLING_FREQUENCY = 16.0;
constexpr double DEFAULT_STEP_SIZE = 0.4;

// Local colors for this view.
const Color ORIGINAL_COLOR{51, 65, 85, 255};      // slate-700
const Color SAMPLE_GRID{148, 163, 184, 55};


// Live regeneration: recompute only when a parameter actually changed.

struct Params {
    double amplitude = 0.0;
    double frequency = 0.0;
    double samplingFrequency = 0.0;
    double stepSize = 0.0;
    int samples = 0;
};

Params currentParams(const AppState& s) {
    return {s.dmAmplitude, s.dmFrequency, s.dmSamplingFrequency,
            s.dmStepSize, s.sequenceLength};
}

bool sameParams(const Params& a, const Params& b) {
    return a.amplitude == b.amplitude && a.frequency == b.frequency &&
           a.samplingFrequency == b.samplingFrequency &&
           a.stepSize == b.stepSize && a.samples == b.samples;
}

Params lastParams;
bool haveLastParams = false;
std::string signalError;

std::string validationError(const AppState& s) {
    if (s.dmAmplitude <= 0.0) return "Amplitude must be positive.";
    if (s.dmFrequency <= 0.0) return "Frequency must be positive.";
    if (s.dmSamplingFrequency <= 0.0) return "Sampling frequency must be positive.";
    if (s.dmStepSize <= 0.0) return "Step size must be positive.";
    if (s.dmFrequency >= s.dmSamplingFrequency / 2.0) {
        return std::string(TextFormat(
            "Frequency must be below fs/2 (Nyquist): fs/2 = %.1f Hz.",
            s.dmSamplingFrequency / 2.0));
    }
    return {};
}

void clearSignal(AppState& s) {
    s.originalSamples.clear();
    s.reconstructedSamples.clear();
    s.dmBits.clear();
}

void syncSignal(AppState& state) {
    const Params now = currentParams(state);
    if (haveLastParams && sameParams(now, lastParams)) return;

    lastParams = now;
    haveLastParams = true;

    signalError = validationError(state);
    if (!signalError.empty()) {
        clearSignal(state);
        return;
    }

    try {
        refreshDeltaModulation(state);
    } catch (const std::exception& e) {
        signalError = e.what();
        clearSignal(state);
    }
}


// Analysis helpers

double niceStep(double raw) {
    static const double steps[] = {0.01, 0.02, 0.05, 0.1, 0.2, 0.5,
                                   1.0, 2.0, 5.0, 10.0, 20.0, 50.0};
    for (double s : steps) {
        if (s >= raw) return s;
    }
    return 50.0;
}


// Plot

void drawPlot(const AppState& state) {
    const Rectangle plot{55, 265, 990, 515};
    DrawRectangleRounded(plot, 0.02f, 8, PLOT_BG);
    DrawRectangleRoundedLines(plot, 0.02f, 8, GRID);

    const auto& original = state.originalSamples;
    const auto& reconstructed = state.reconstructedSamples;

    if (original.empty() || reconstructed.size() != original.size()) {
        drawText("No signal to display", plot.x + 20, plot.y + 20, 18, MUTED);
        return;
    }

    const std::size_t n = original.size();
    const double fs = state.dmSamplingFrequency;
    const double duration = static_cast<double>(n) / fs;

    const float left = plot.x + 60;
    const float right = plot.x + plot.width - 25;
    const float top = plot.y + 75;
    const float bottom = plot.y + plot.height - 75;
    const float middle = (top + bottom) / 2.0f;
    const float halfHeight = (bottom - top) / 2.0f;
    const float width = right - left;
    const float dx = width / static_cast<float>(n);

    // Vertical range with round grid values.
    double maxMagnitude = std::max(state.dmAmplitude, 0.1);
    for (double v : original) maxMagnitude = std::max(maxMagnitude, std::abs(v));
    for (double v : reconstructed) maxMagnitude = std::max(maxMagnitude, std::abs(v));

    const double gridStep = niceStep(maxMagnitude / 3.0);
    const double range = gridStep * std::ceil(maxMagnitude / gridStep - 1e-9);

    auto mapY = [&](double value) {
        return middle - static_cast<float>(value / range) * halfHeight;
    };

    // Horizontal grid with amplitude labels.
    const int lines = static_cast<int>(std::lround(range / gridStep));
    const char* labelFormat =
        gridStep >= 1.0 ? "%.0f" : (gridStep >= 0.1 ? "%.1f" : "%.2f");

    for (int i = -lines; i <= lines; i++) {
        const double value = static_cast<double>(i) * gridStep;
        const float y = mapY(value);
        DrawLineEx({left, y}, {right, y}, i == 0 ? 1.5f : 1.0f,
                   i == 0 ? GRID_STRONG : GRID);

        const char* label = TextFormat(labelFormat, value);
        drawText(label, left - 8 - textWidth(label, 12), y - 7, 12, MUTED);
    }



    // Sample instants and time axis.
        const std::size_t stride = static_cast<std::size_t>(
        std::max(1.0f, std::ceil(46.0f / dx)));

    for (std::size_t k = 0; k <= n; k++) {
        const float x = left + static_cast<float>(k) * dx;
        DrawLineEx({x, top}, {x, bottom}, 1.0f, SAMPLE_GRID);
        DrawLineEx({x, bottom}, {x, bottom + 5}, 1.0f, MUTED);

        if (k % stride == 0) {
            const char* label = TextFormat("%.2f", static_cast<double>(k) / fs);
            drawText(label, x - textWidth(label, 11) / 2.0f, bottom + 9, 11, MUTED);
        }
    }
    drawText("Time (s)", right - textWidth("Time (s)", 12), bottom + 24, 12, MUTED);

    // Reconstructed staircase.
    for (std::size_t k = 0; k < n; k++) {
        const float x1 = left + static_cast<float>(k) * dx;
        const float x2 = x1 + dx;
        const float y = mapY(reconstructed[k]);

        DrawLineEx({x1, y}, {x2, y}, 3.0f, SIGNAL);
        if (k + 1 < n) {
            DrawLineEx({x2, y}, {x2, mapY(reconstructed[k + 1])}, 3.0f, SIGNAL);
        }
    }

    // Original signal: dense curve, not a polyline through the samples.
    constexpr int segments = 320;
    Vector2 previous{left, mapY(0.0)};
    for (int s = 1; s <= segments; s++) {
        const double fraction = static_cast<double>(s) / segments;
        const double t = duration * fraction;
        const Vector2 point{
            left + static_cast<float>(fraction) * width,
            mapY(state.dmAmplitude * std::sin(2.0 * pi * state.dmFrequency * t))
        };
        DrawLineEx(previous, point, 2.0f, ORIGINAL_COLOR);
        previous = point;
    }

    // Sample dots on the original.
    for (std::size_t k = 0; k < n; k++) {
        DrawCircleV({left + static_cast<float>(k) * dx, mapY(original[k])},
                    4.0f, ORIGINAL_COLOR);
    }

    // Encoded bits aligned with their sample intervals.
    drawText("Bits", plot.x + 12, plot.y + 46, 12, MUTED);
    const std::size_t bitCount = std::min(n, state.dmBits.size());
    for (std::size_t k = 0; k < bitCount; k++) {
        const bool one = state.dmBits[k] != 0;
        const char* label = one ? "1" : "0";
        const float w = textWidth(label, 15);
        drawBoldText(label,
                     left + static_cast<float>(k) * dx + dx / 2.0f - w / 2.0f,
                     plot.y + 44, 15, one ? SIGNAL : MUTED);
    }

    // Legend.
    float lx = left;
    const float ly = plot.y + 18;
    auto legendText = [&](const char* text) {
        drawText(text, lx + 36, ly - 8, 13, TEXT_PRIMARY);
        lx += 36 + textWidth(text, 13) + 28;
    };

    DrawLineEx({lx, ly}, {lx + 28, ly}, 2.0f, ORIGINAL_COLOR);
    legendText("Original signal");

    DrawLineEx({lx, ly}, {lx + 28, ly}, 3.0f, SIGNAL);
    legendText("Reconstructed staircase");

    DrawCircleV({lx + 14, ly}, 4.0f, ORIGINAL_COLOR);
    legendText("Sample instants");


    // Error metrics (reconstructed vs original, per sample).
    double maxError = 0.0;
    double sumError = 0.0;
    for (std::size_t k = 0; k < n; k++) {
        const double e = std::abs(reconstructed[k] - original[k]);
        maxError = std::max(maxError, e);
        sumError += e;
    }
    const double meanError = sumError / static_cast<double>(n);

drawText(TextFormat("Max error: %.3f     Mean abs error: %.3f     Duration: %.3f s",
                    maxError, meanError, duration),
         left, plot.y + plot.height - 34, 14, MUTED);
}

} // namespace

void refreshDeltaModulation(AppState& state) {
    state.originalSamples = sampleSineWave(
        state.dmAmplitude,
        state.dmFrequency,
        state.dmSamplingFrequency,
        static_cast<std::size_t>(state.sequenceLength)
    );

    state.dmBits = encodeDeltaModulation(
        state.originalSamples,
        state.dmStepSize
    );

    state.reconstructedSamples = decodeDeltaModulation(
        state.dmBits,
        state.dmStepSize
    );
}

void drawDeltaModulationPanel(AppState& state) {
    drawBoldText("DELTA MODULATION", 45, 104, 17, MUTED);

    // Parameters (editable) 
    constexpr float labelY = 135;
    constexpr float rowY = 154;
    constexpr float rowH = 36;

    drawText("Amplitude", 45, labelY, 13, MUTED);
    decimalField(10, {45, rowY, 150, rowH}, state.dmAmplitude,
                 0.1, 5.0, 0.1, 1);

    drawText("Frequency (Hz)", 215, labelY, 13, MUTED);
    decimalField(11, {215, rowY, 150, rowH}, state.dmFrequency,
                 0.1, 50.0, 0.1, 1);

    drawText("Sampling freq. (Hz)", 385, labelY, 13, MUTED);
    decimalField(12, {385, rowY, 150, rowH}, state.dmSamplingFrequency,
                 1.0, 200.0, 1.0, 1);

    drawText("Step size", 555, labelY, 13, MUTED);
    decimalField(13, {555, rowY, 150, rowH}, state.dmStepSize,
                 0.01, 2.0, 0.01, 2);

    drawText("Samples", 725, labelY, 13, MUTED);
    numberField(14, {725, rowY, 150, rowH}, state.sequenceLength,
                1, MAX_SAMPLES);

    if (button({900, rowY, 150, rowH}, "Reset defaults")) {
        commitEdit();
        state.dmAmplitude = DEFAULT_AMPLITUDE;
        state.dmFrequency = DEFAULT_FREQUENCY;
        state.dmSamplingFrequency = DEFAULT_SAMPLING_FREQUENCY;
        state.dmStepSize = DEFAULT_STEP_SIZE;
    }

    // Recompute whenever a parameter changed (this frame or elsewhere).
    syncSignal(state);

  // Display validation errors, if any.
if (!signalError.empty()) {
    drawText(signalError.c_str(), 45, 203, 14, ERROR_COLOR);
}

    //Encoded bits
    drawText("Encoded bits:", 45, 233, 14, MUTED);

    std::string bitText;
    for (std::uint8_t bit : state.dmBits) {
        bitText += bit ? '1' : '0';
        bitText += ' ';
    }
    drawText(bitText.c_str(), 150, 231, 16, TEXT_PRIMARY);

    drawPlot(state);
}

} 