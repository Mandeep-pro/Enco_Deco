#ifndef ENCO_DECO_DELTA_MODULATION_HPP
#define ENCO_DECO_DELTA_MODULATION_HPP

#include "types.hpp"
#include <vector>

// will try to decode using graph later
namespace encodeco{
    Bits encodeDeltaModulation(const std::vector<double> &samples, double stepSize, double initialValue =0.0);
    std::vector<double> decodeDeltaModulation(const Bits &bits, double stepSize, double initialValue = 0.0);
}

#endif