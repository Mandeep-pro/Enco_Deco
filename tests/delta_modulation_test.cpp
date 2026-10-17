
#include <cassert>
#include <iostream>
#include <vector>

#include "delta_modulation.hpp"

using namespace encodeco;

int main() {
    
    // Step size: 1
    // Initial staircase: 0
    std::vector<double> samples = {1,2,3,4,5,6,7,8,7,6,5,4,3,2,1,0};

    Bits expected = {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0};

    assert(encodeDeltaModulation(samples, 1.0) == expected);

    // Empty input.
    assert(encodeDeltaModulation({}, 1.0).empty());

    // Invalid step size.
    bool threw = false;

    try {
        encodeDeltaModulation(samples, 0.0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    assert(threw);

    std::cout << "Delta Modulation encoder tests passed!\n";


    // Decoder test

    Bits bits = {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0};
    std::vector<double> expectedSamples = {1,2,3,4,5,6,7,8,7,6,5,4,3,2,1,0};
    assert(decodeDeltaModulation(bits,1.0) == expectedSamples); // giving bits to decoder gives the sample
    assert(encodeDeltaModulation(expectedSamples,1.0)==bits); // encoding this sample gives the bits

    std::cout << "Delta Modulation decoder tests passed!\n";
    return 0;

    
}
