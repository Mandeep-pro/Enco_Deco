
#include <cassert>
#include <cmath>
#include <iostream>

#include "analog_signal_generator.hpp"

using namespace encodeco;

int main() {

    // A = 8, frequency = 1Hz, sampling frequency = 32Hz, samples = 64
    auto samples = sampleSineWave(8.0, 1.0, 32.0, 64);
    for(auto sample:samples)
    std::cout <<  sample << ",";
    std::cout << "\n";
    // Must produce the requested number of samples.
    assert(samples.size() == 64);

    // A sine wave starts at zero.
    assert(std::abs(samples[0]) < 1e-9);

    // The sample at one quarter of a cycle should be near +8(peak commes at 90) time = 8/32 = 0.25 , value = 8 sin(2*3.14*1*0.25) = 8 sin (pi/2) = 8 (approx)
    assert(std::abs(samples[8] - 8.0) < 1e-9);

    std::cout << "Signal generator tests passed.\n";
    return 0;
}
