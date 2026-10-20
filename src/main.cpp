#include "raylib.h"

#include "app_state.hpp"
#include "input_panel.hpp"
#include "ui_helpers.hpp"
#include "waveform_renderer.hpp"
#include "delta_modulation_panel.hpp"

namespace encodeco {
namespace {
constexpr int WINDOW_WIDTH = 1100;
constexpr int WINDOW_HEIGHT = 860;

constexpr int AMI_INDEX = 4;

const char* encodingNames[] = {"NRZ-L", "NRZ-I", "Manchester", "Differential Manchester", "AMI"};

const char* scramblerNames[] = {"None", "B8ZS", "HDB3"};

// Single entry point for changing the scheme (buttons and hotkeys).
// Scrambling is AMI-only, so leaving AMI resets the scrambler to None.
void selectEncoding(AppState& state, int index) {
    state.selectedEncoding = index;
    if (index != AMI_INDEX) state.selectedScrambler = 0;
    refreshEncoding(state);
}

void drawEncodingPanel(AppState& state) {
    drawBoldText("ENCODING SCHEME", 45, 292, 17, MUTED);
    constexpr float buttonWidth = 190;
    constexpr float gap = 14;

    for (int i = 0; i < 5; i++) {
        const Rectangle rect{45 + i * (buttonWidth + gap), 320, buttonWidth, 45};
        if (button(rect, encodingNames[i], state.selectedEncoding == i))selectEncoding(state, i);
 }

    const bool isAmi = (state.selectedEncoding == AMI_INDEX);

    const char* activeLabel =(isAmi && state.selectedScrambler != 0)? TextFormat("ACTIVE ENCODING: AMI + %s",scramblerNames[state.selectedScrambler]): TextFormat("ACTIVE ENCODING: %s", 
    encodingNames[state.selectedEncoding]);
    drawText(activeLabel, 45, 380, 18, TEXT_PRIMARY);

    // Scrambler options: only while AMI is selected, placed to the right of
    // the active-encoding label so the two never overlap.
    if (isAmi) {
        drawText("SCRAMBLER", 420, 385, 13, MUTED);

        for (int i = 0; i < 3; i++) {
            const Rectangle rect{510.0f + i * 98.0f, 376, 90, 28};
            if (button(rect, scramblerNames[i], state.selectedScrambler == i)) {
                state.selectedScrambler = i;
                refreshEncoding(state);
            }
        }
    }
}
} }

int main() {
    using namespace encodeco;
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Enco_Deco | Signal Lab");
    SetExitKey(KEY_NULL);
    initializeUI();
    SetTargetFPS(60);

    AppState state;
    generateSequence(state);
    bool quit = false;

    while (!quit && !WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (isEditing()) cancelEdit();
            else quit = true;
        }

        if (!isEditing()) {
            while (GetCharPressed() > 0) {}
            for (int i = 0; i < 5; i++) {
                if (IsKeyPressed(KEY_ONE + i)) selectEncoding(state, i);
            }
        }

       
BeginDrawing();
ClearBackground(BACKGROUND);

drawBoldText("Enco_Deco", 35, 16, 38, TEXT_PRIMARY);
drawText("SIGNAL LAB", 37, 60, 13, MUTED);

if (button({650, 30, 180, 42}, "Digital Mode",
           !state.deltaModulationMode)) {
    state.deltaModulationMode = false;
}

if (button({840, 30, 210, 42}, "Analog Mode",
           state.deltaModulationMode)) {
    state.deltaModulationMode = true;
}

DrawLine(35, 88, 1065, 88, GRID);

if (state.deltaModulationMode) {
    drawDeltaModulationPanel(state);
} else {
    drawInputPanel(state);
    drawEncodingPanel(state);
    drawBoldText("ENCODED WAVEFORM", 45, 413, 17, MUTED);
    drawWaveform(state);

    drawText(
        "Keys 1-5: change encoding    |    Enter: apply value    |    Esc: cancel edit / exit",
        45, 818, 15, MUTED
    );
}

EndDrawing();

    }

    shutdownUI();
    CloseWindow();
    return 0;
}