#include "ui_helpers.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>

namespace encodeco {
namespace {
constexpr const char* REGULAR_FONT_PATH = "/usr/share/fonts/google-noto/NotoSans-Regular.ttf";
constexpr const char* BOLD_FONT_PATH = "/usr/share/fonts/google-noto/NotoSans-Bold.ttf";

Font uiFont{};
Font boldFont{};
bool regularLoaded = false;
bool boldLoaded = false;

// State of the field currently being typed into (integer or decimal).
struct EditState {
    int id = -1;

    // Integer field.
    int* target = nullptr;
    int minValue = 0;
    int maxValue = 0;

    // Decimal field.
    double* decimalTarget = nullptr;
    double minDecimal = 0.0;
    double maxDecimal = 0.0;
    int decimals = 0;
    bool fresh = false;   // first typed key replaces the prefilled text

    std::string buffer;
} edit;

void clearEdit() {
    edit.id = -1;
    edit.target = nullptr;
    edit.decimalTarget = nullptr;
    edit.fresh = false;
    edit.buffer.clear();
}

std::string formatDecimal(double value, int decimals) {
    char text[32];
    std::snprintf(text, sizeof(text), "%.*f", decimals, value);
    return text;
}

void beginEdit(int id, int& value, int minValue, int maxValue) {
    commitEdit();
    edit.id = id;
    edit.target = &value;
    edit.minValue = minValue;
    edit.maxValue = maxValue;
    edit.buffer = std::to_string(value);
}

void beginDecimalEdit(int id, double& value, double minValue, double maxValue,
                      int decimals) {
    commitEdit();
    edit.id = id;
    edit.decimalTarget = &value;
    edit.minDecimal = minValue;
    edit.maxDecimal = maxValue;
    edit.decimals = decimals;
    edit.fresh = true;
    edit.buffer = formatDecimal(value, decimals);
}
} // namespace

const Color BACKGROUND{244, 246, 250, 255};
const Color PANEL{255, 255, 255, 255};
const Color PANEL_HOVER{232, 237, 245, 255};
const Color PLOT_BG{255, 255, 255, 255};
const Color SIGNAL{29, 78, 216, 255};
const Color GRID{148, 163, 184, 120};
const Color GRID_STRONG{100, 116, 139, 200};
const Color GRID_HALF{148, 163, 184, 150};
const Color ACCENT{37, 99, 235, 255};
const Color MUTED{100, 116, 139, 255};
const Color TEXT_PRIMARY{15, 23, 42, 255};
const Color TEXT_ON_ACCENT{255, 255, 255, 255};
const Color ERROR_COLOR{185, 28, 28, 255};

void initializeUI() {
    uiFont = LoadFontEx(REGULAR_FONT_PATH, 32, nullptr, 0);
    if (uiFont.texture.id == 0) {
        uiFont = GetFontDefault();
    } else {
        regularLoaded = true;
        SetTextureFilter(uiFont.texture, TEXTURE_FILTER_BILINEAR);
    }

    boldFont = LoadFontEx(BOLD_FONT_PATH, 48, nullptr, 0);
    if (boldFont.texture.id == 0) {
        boldFont = uiFont;
    } else {
        boldLoaded = true;
        SetTextureFilter(boldFont.texture, TEXTURE_FILTER_BILINEAR);
    }
}

void shutdownUI() {
    if (boldLoaded) UnloadFont(boldFont);
    if (regularLoaded) UnloadFont(uiFont);
}

void drawText(const char* text, float x, float y, float fontSize, Color color) {
    DrawTextEx(uiFont, text, {x, y}, fontSize, 1.0f, color);
}

void drawBoldText(const char* text, float x, float y, float fontSize, Color color) {
    DrawTextEx(boldFont, text, {x, y}, fontSize, 1.0f, color);
}

float textWidth(const char* text, float fontSize) {
    return MeasureTextEx(uiFont, text, fontSize, 1.0f).x;
}

bool button(Rectangle rect, const char* label, bool active, bool enabled) {
    const Vector2 mouse = GetMousePosition();
    const bool hovered = enabled && CheckCollisionPointRec(mouse, rect);

    Color fill = active ? ACCENT : PANEL;
    Color textColor = active ? TEXT_ON_ACCENT : TEXT_PRIMARY;
    Color border = active ? ACCENT : GRID;

    if (!enabled) {
        fill = PANEL_HOVER;
        textColor = MUTED;
        border = GRID;
    } else if (hovered && !active) {
        fill = PANEL_HOVER;
    }

    DrawRectangleRounded(rect, 0.18f, 8, fill);
    DrawRectangleRoundedLines(rect, 0.18f, 8, border);

    constexpr float fontSize = 16.0f;
    const float labelWidth = textWidth(label, fontSize);
    drawText(label, rect.x + (rect.width - labelWidth) / 2.0f,
             rect.y + (rect.height - fontSize) / 2.0f,
             fontSize, textColor);

    return hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

void commitEdit() {
    if (edit.id >= 0 && !edit.buffer.empty()) {
        try {
            if (edit.decimalTarget != nullptr) {
                const double parsed = std::stod(edit.buffer);
                const double scale = std::pow(10.0, edit.decimals);
                *edit.decimalTarget = std::clamp(std::round(parsed * scale) / scale,
                                                 edit.minDecimal, edit.maxDecimal);
            } else if (edit.target != nullptr) {
                const int parsed = std::stoi(edit.buffer);
                *edit.target = std::clamp(parsed, edit.minValue, edit.maxValue);
            }
        } catch (...) {
            // Ignore invalid input and preserve the previous value.
        }
    }
    clearEdit();
}

void cancelEdit() {
    clearEdit();
}

bool isEditing() {
    return edit.id >= 0;
}

void numberField(int id, Rectangle rect, int& value, int minValue,
                 int maxValue, bool enabled) {
    constexpr float stepWidth = 32.0f;
    const Rectangle minus{rect.x, rect.y, stepWidth, rect.height};
    const Rectangle plus{rect.x + rect.width - stepWidth, rect.y,
                         stepWidth, rect.height};
    const Rectangle field{rect.x + stepWidth + 4, rect.y,
                          rect.width - 2 * stepWidth - 8, rect.height};

    if (!enabled && edit.id == id) cancelEdit();

    const bool pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    const bool inField = CheckCollisionPointRec(GetMousePosition(), field);
    if (enabled && pressed && inField && edit.id != id) {
        beginEdit(id, value, minValue, maxValue);
    } else if (edit.id == id && pressed && !inField) {
        commitEdit();
    }

    if (edit.id == id) {
        const std::size_t maxDigits = std::to_string(maxValue).size();
        int c = GetCharPressed();
        while (c > 0) {
            if (c >= '0' && c <= '9' && edit.buffer.size() < maxDigits) {
                edit.buffer.push_back(static_cast<char>(c));
            }
            c = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !edit.buffer.empty()) edit.buffer.pop_back();
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) commitEdit();
    }

    if (button(minus, "-", false, enabled)) {
        if (edit.id == id) commitEdit();
        value = std::max(minValue, value - 1);
    }
    if (button(plus, "+", false, enabled)) {
        if (edit.id == id) commitEdit();
        value = std::min(maxValue, value + 1);
    }

    const bool focused = edit.id == id;
    DrawRectangleRounded(field, 0.18f, 8, enabled ? PANEL : PANEL_HOVER);
    DrawRectangleRoundedLines(field, 0.18f, 8, focused ? ACCENT : GRID);

    const std::string valueText = focused ? edit.buffer : std::to_string(value);
    constexpr float fontSize = 18.0f;
    const float width = textWidth(valueText.c_str(), fontSize);
    const float tx = field.x + (field.width - width) / 2.0f;
    const float ty = field.y + (field.height - fontSize) / 2.0f;
    drawText(valueText.c_str(), tx, ty, fontSize, enabled ? TEXT_PRIMARY : MUTED);

    if (focused && std::fmod(GetTime(), 1.0) < 0.5) {
        DrawLineEx({tx + width + 2, ty}, {tx + width + 2, ty + fontSize},
                   1.5f, TEXT_PRIMARY);
    }
}

// [-] [ value ] [+] for decimal values. Click the value to type digits and
// one '.', the first key replaces the old text. Enter or click-away applies.
void decimalField(int id, Rectangle rect, double& value, double minValue,
                  double maxValue, double step, int decimals, bool enabled) {
    constexpr float stepWidth = 32.0f;
    constexpr std::size_t maxChars = 8;
    const Rectangle minus{rect.x, rect.y, stepWidth, rect.height};
    const Rectangle plus{rect.x + rect.width - stepWidth, rect.y,
                         stepWidth, rect.height};
    const Rectangle field{rect.x + stepWidth + 4, rect.y,
                          rect.width - 2 * stepWidth - 8, rect.height};

    if (!enabled && edit.id == id) cancelEdit();

    const bool pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    const bool inField = CheckCollisionPointRec(GetMousePosition(), field);
    if (enabled && pressed && inField && edit.id != id) {
        beginDecimalEdit(id, value, minValue, maxValue, decimals);
    } else if (edit.id == id && pressed && !inField) {
        commitEdit();
    }

    if (edit.id == id) {
        int c = GetCharPressed();
        while (c > 0) {
            const bool isDigit = c >= '0' && c <= '9';
            const bool isDot = c == '.';
            if (isDigit || isDot) {
                if (edit.fresh) {
                    edit.buffer.clear();
                    edit.fresh = false;
                }
                const bool duplicateDot =
                    isDot && edit.buffer.find('.') != std::string::npos;
                if (!duplicateDot && edit.buffer.size() < maxChars) {
                    edit.buffer.push_back(static_cast<char>(c));
                }
            }
            c = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (edit.fresh) {
                edit.buffer.clear();
                edit.fresh = false;
            } else if (!edit.buffer.empty()) {
                edit.buffer.pop_back();
            }
        }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) commitEdit();
    }

    const double scale = std::pow(10.0, decimals);
    auto stepBy = [&](double delta) {
        if (edit.id == id) commitEdit();
        value = std::clamp(std::round((value + delta) * scale) / scale,
                           minValue, maxValue);
    };

    if (button(minus, "-", false, enabled)) stepBy(-step);
    if (button(plus, "+", false, enabled)) stepBy(step);

    const bool focused = edit.id == id;
    DrawRectangleRounded(field, 0.18f, 8, enabled ? PANEL : PANEL_HOVER);
    DrawRectangleRoundedLines(field, 0.18f, 8, focused ? ACCENT : GRID);

    const std::string valueText =
        focused ? edit.buffer : formatDecimal(value, decimals);
    constexpr float fontSize = 18.0f;
    const float width = textWidth(valueText.c_str(), fontSize);
    const float tx = field.x + (field.width - width) / 2.0f;
    const float ty = field.y + (field.height - fontSize) / 2.0f;
    drawText(valueText.c_str(), tx, ty, fontSize, enabled ? TEXT_PRIMARY : MUTED);

    if (focused && std::fmod(GetTime(), 1.0) < 0.5) {
        DrawLineEx({tx + width + 2, ty}, {tx + width + 2, ty + fontSize},
                   1.5f, TEXT_PRIMARY);
    }
}

} // namespace encodeco