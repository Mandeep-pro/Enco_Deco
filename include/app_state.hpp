#ifndef ENCO_DECO_APP_STATE_HPP
#define ENCO_DECO_APP_STATE_HPP

#include "types.hpp"
#include <string>

namespace encodeco {

struct AppState {
    Bits bits;
    Levels levels;
    std::string palindromeText;
 

    int selectedEncoding = 0;
    int sequenceLength = 16;
    int generationMode = 0; // 0 = random, 1 = planted zero-runs
    int runLength = 4;
    int runCount = 2;

     int selectedScrambler = 0; // 0 = none, 1 = B8ZS, 2 = HDB3
   // Delta Modulation state

     bool deltaModulationMode = false;
double dmAmplitude = 1.0;
double dmFrequency = 1.0;
double dmSamplingFrequency = 16.0;
double dmStepSize = 0.2;

std::vector<double> originalSamples;
std::vector<double> reconstructedSamples;
Bits dmBits;
};

} // namespace encodeco

#endif
