#include "delta_modulation.hpp"
#include "analog_signal_generator.hpp"
#include <cassert>

#include <iostream>
using namespace encodeco;
int main(){
    // generate samples from sine wave with A = 8, f = 1Hz, fs = 32Hz, no. of samples = 64
    auto samples = sampleSineWave(8.0,1.0,32.0,64);

    // feed generated samples into encoder to get bits according to Delta Modulation scheme stepSize = 1.
    auto bits = encodeDeltaModulation(samples,1.0);
    // reconstructed samples will show some loss
    auto result = decodeDeltaModulation(bits,1.0);

    
    assert(samples.size() == bits.size() && bits.size() == result.size() && result.size() == 64);

    for (std::size_t i = 0; i < samples.size(); ++i) {
    std::cout << samples[i] << "\t"  << static_cast<int>(bits[i]) << "\t"<< result[i] << '\n';
}


}