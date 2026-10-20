#ifndef ENCO_DECO_UI_HELPERS_HPP
#define ENCO_DECO_UI_HELPERS_HPP

#include "raylib.h"

namespace encodeco {

void initializeUI();
void shutdownUI();
void drawText(const char* text, float x, float y, float fontSize, Color color);
void drawBoldText(const char* text, float x, float y, float fontSize, Color color);
float textWidth(const char* text, float fontSize);
bool button(Rectangle rect, const char* label, bool active = false,
            bool enabled = true);

            void decimalField(int id, Rectangle rect, double& value,
                  double minValue, double maxValue,
                  double step, int decimals, bool enabled = true);
void commitEdit();
void cancelEdit();
bool isEditing();
void numberField(int id, Rectangle rect, int& value, int minValue,
                 int maxValue, bool enabled = true);

// UI colors shared by the main window and panels.
extern const Color BACKGROUND;
extern const Color PANEL;
extern const Color PANEL_HOVER;
extern const Color PLOT_BG;
extern const Color SIGNAL;
extern const Color GRID;
extern const Color GRID_STRONG;
extern const Color GRID_HALF;
extern const Color ACCENT;
extern const Color MUTED;
extern const Color TEXT_PRIMARY;
extern const Color TEXT_ON_ACCENT;
extern const Color ERROR_COLOR;

} // namespace encodeco

#endif
