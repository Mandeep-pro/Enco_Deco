#ifndef ENCO_DECO_SEQUENCE_GENERATOR
#define ENCO_DECO_SEQUENCE_GENERATOR

#include "types.hpp"
#include <iostream>
#include <cstddef>
#include <cstdint>
#include <random>
namespace encodeco{
    Bits generateRandomBits(std::size_t length);
    Bits generateBitsWithZeroRuns(std::size_t length, std::size_t zeroRunLength, std::size_t runCount);
    // length of sequence -> length,  length of zero runs -> zeroRunLength, no. of such zero runs -> runCount
}



#endif