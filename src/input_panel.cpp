#include "input_panel.hpp"

#include "ami_scrambler.hpp"
#include "line_encoder.hpp"
#include "palindrome.hpp"
#include "sequence_generator.hpp"
#include "ui_helpers.hpp"

#include <cstddef>
#include <cstdint>
#include <string>

namespace encodeco {
namespace {
constexpr int MAX_LENGTH = 32;
constexpr int MAX_RUN_COUNT = 16;

// Generation modes (AppState::generationMode).
constexpr int MODE_RANDOM = 0;
constexpr int MODE_ZERO_RUNS = 1;
constexpr int MODE_MANUAL = 2;

// Zero-run lengths that trigger a scrambler substitution.
constexpr int RUN_LENGTH_HDB3 = 4;
constexpr int RUN_LENGTH_B8ZS = 8;

std::string bitsToString(const Bits& bits) {
    std::string result;
    result.reserve(bits.size() * 2);
    for (std::uint8_t bit : bits) {
        result += bit == 0 ? '0' : '1';
        result += ' ';
    }
    return result;
}

// "1011" form, no separators (used by the manual editor).
std::string bitsToPlainString(const Bits& bits) {
    std::string result;
    result.reserve(bits.size());
    for (std::uint8_t bit : bits) result += bit == 0 ? '0' : '1';
    return result;
}

Bits parseBitString(const std::string& text) {
    Bits result;
    result.reserve(text.size());
    for (char c : text) {
        if (c == '0') result.push_back(0);
        else if (c == '1') result.push_back(1);
    }
    return result;
}

Levels encodeSelected(const AppState& state) {
    // Scramblers apply to AMI only.
    if (state.selectedEncoding == 4) {
        if (state.selectedScrambler == 1) return scrambleB8ZS(state.bits);
        if (state.selectedScrambler == 2) return scrambleHDB3(state.bits);
        return encodeAMI(state.bits);
    }

    switch (state.selectedEncoding) {
        case 0: return encodeNRZL(state.bits);
        case 1: return encodeNRZI(state.bits);
        case 2: return encodeManchester(state.bits);
        case 3: return encodeDiffManchester(state.bits);
        default: return encodeAMI(state.bits);
    }
}
} // namespace

bool zeroRunsFit(const AppState& state) {
    return state.runLength * state.runCount <= state.sequenceLength;
}

bool canGenerate(const AppState& state) {
    if (state.generationMode == MODE_RANDOM) return true;

    if (state.generationMode == MODE_MANUAL) {
        return !state.manualBits.empty();
    }

    const bool validRunLength = state.runLength == RUN_LENGTH_HDB3 ||
                                state.runLength == RUN_LENGTH_B8ZS;

    return validRunLength && zeroRunsFit(state);
}

void refreshEncoding(AppState& state) {
    state.levels = encodeSelected(state);

    const Palindrome p = findLongestPalindrome(state.bits);
    state.palindromeText = "Longest palindrome: length " +
        std::to_string(p.length) + ", starting at index " +
        std::to_string(p.start);
}

void generateSequence(AppState& state) {
    if (!canGenerate(state)) return;

    if (state.generationMode == MODE_RANDOM) {
        state.bits = generateRandomBits(
            static_cast<std::size_t>(state.sequenceLength));
    } else if (state.generationMode == MODE_ZERO_RUNS) {
        state.bits = generateBitsWithZeroRuns(
            static_cast<std::size_t>(state.sequenceLength),
            static_cast<std::size_t>(state.runLength),
            static_cast<std::size_t>(state.runCount));
    } else {
        // Manual: the typed string is the sequence; length follows it.
        state.bits = parseBitString(state.manualBits);
        state.sequenceLength = static_cast<int>(state.bits.size());
    }
    refreshEncoding(state);
}

void drawInputPanel(AppState& state) {
    drawBoldText("INPUT SEQUENCE", 45, 104, 17, MUTED);

    const std::string bitsText = bitsToString(state.bits);
    constexpr float maxBitsWidth = 1005.0f;
    float bitsFont = 25.0f;
    const float bitsWidth = textWidth(bitsText.c_str(), bitsFont);
    if (bitsWidth > maxBitsWidth) bitsFont *= maxBitsWidth / bitsWidth;
    drawText(bitsText.c_str(), 45, 133, bitsFont, TEXT_PRIMARY);
    drawText(state.palindromeText.c_str(), 45, 170, 17, SIGNAL);

    constexpr float labelY = 205;
    constexpr float rowY = 224;
    constexpr float rowH = 40;
    const bool zeroRunMode = state.generationMode == MODE_ZERO_RUNS;
    const bool manualMode = state.generationMode == MODE_MANUAL;
    const Color runLabelColor = zeroRunMode ? MUTED : GRID_STRONG;

    // Sequence length (follows the typed string in manual mode).
    drawText("Sequence length", 45, labelY, 13, MUTED);
    numberField(0, {45, rowY, 130, rowH}, state.sequenceLength, 1, MAX_LENGTH,
                !manualMode);

    // Generation mode.
    drawText("Generation mode", 205, labelY, 13, MUTED);
    if (button({205, rowY, 100, rowH}, "Random",
               state.generationMode == MODE_RANDOM)) {
        commitEdit();
        state.generationMode = MODE_RANDOM;
    }
    if (button({311, rowY, 112, rowH}, "Zero-runs", zeroRunMode)) {
        commitEdit();
        state.generationMode = MODE_ZERO_RUNS;
    }
    if (button({429, rowY, 90, rowH}, "Manual", manualMode)) {
        commitEdit();
        // First time in: start from the sequence currently on screen.
        if (state.manualBits.empty()) {
            state.manualBits = bitsToPlainString(state.bits);
        }
        state.generationMode = MODE_MANUAL;
    }

    bool submitted = false;

    if (manualMode) {
        // Custom bit string.
        drawText("Custom bits (0/1)", 549, labelY, 13, MUTED);

        const char* counter = TextFormat("%d / %d",
            static_cast<int>(state.manualBits.size()), MAX_LENGTH);
        drawText(counter, 835 - textWidth(counter, 13), labelY, 13, MUTED);

        submitted = bitStringField(3, {549, rowY, 286, rowH}, state.manualBits,
                                   static_cast<std::size_t>(MAX_LENGTH));
    } else {
        // Zero-run length: 4 (HDB3) or 8 (B8ZS).
        drawText("Zero-run length", 549, labelY, 13, runLabelColor);
        if (button({549, rowY, 60, rowH}, "4",
                   state.runLength == RUN_LENGTH_HDB3, zeroRunMode)) {
            commitEdit();
            state.runLength = RUN_LENGTH_HDB3;
        }
        if (button({615, rowY, 60, rowH}, "8",
                   state.runLength == RUN_LENGTH_B8ZS, zeroRunMode)) {
            commitEdit();
            state.runLength = RUN_LENGTH_B8ZS;
        }

        // Zero-run count.
        drawText("Zero-run count", 705, labelY, 13, runLabelColor);
        numberField(2, {705, rowY, 130, rowH}, state.runCount,
                    1, MAX_RUN_COUNT, zeroRunMode);
    }

    // Generate / Apply (Enter in the custom-bits field also applies).
    const char* actionLabel = manualMode ? "Apply" : "Generate";
    if (button({850, rowY, 200, rowH}, actionLabel, true, canGenerate(state)) ||
        (submitted && canGenerate(state))) {
        commitEdit();
        generateSequence(state);
    }

    if (manualMode) {
        drawText("Type 0 or 1, or paste with Ctrl+V. Enter or Apply updates "
                 "the waveform.",
                 45, 270, 13, MUTED);
    } else if (zeroRunMode && !zeroRunsFit(state)) {
        drawText(TextFormat("Zero-runs do not fit: run length x count = %d "
                            "exceeds sequence length %d.",
                            state.runLength * state.runCount,
                            state.sequenceLength),
                 45, 270, 13, ERROR_COLOR);
    }
}

} // namespace encodeco