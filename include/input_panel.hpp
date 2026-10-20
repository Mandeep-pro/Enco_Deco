#ifndef ENCO_DECO_INPUT_PANEL_HPP
#define ENCO_DECO_INPUT_PANEL_HPP

#include "app_state.hpp"

namespace encodeco {

void generateSequence(AppState& state);
void refreshEncoding(AppState& state);
void drawInputPanel(AppState& state);
bool zeroRunsFit(const AppState& state);
bool canGenerate(const AppState& state);

} // namespace encodeco

#endif
