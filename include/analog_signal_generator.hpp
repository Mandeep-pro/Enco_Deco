#ifndef ENCO_DECO_ANALOG_SIGNAL_GENERATOR
#define ENCO_DECO_ANALOG_SIGNAL_GENERATOR

#include "types.hpp"
#include <cstddef>
#include <vector>


namespace encodeco{

    std::vector<double> sampleSineWave(double amplitude, double frequency, double samplingFrequency, std::size_t sampleCount);
    
}

#endif