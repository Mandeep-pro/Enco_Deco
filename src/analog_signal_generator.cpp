#include "analog_signal_generator.hpp"
#include <cmath> // for std::sin
#include <stdexcept>

namespace encodeco{
    // sampling the sine wave by just evaulating the value of Asin 2*pi*f*t at equally spaced t 
    std::vector<double> sampleSineWave(double amplitude, double frequency, double samplingFrequency, std::size_t sampleCount)
    {
        // Nyquist sampling theorem -> fs>2f -> f<fs/2 hence f>=fs/2 is invalid
        if(amplitude < 0.0 || frequency < 0.0 || samplingFrequency <=0.0 || frequency>=samplingFrequency/2.0) throw std::invalid_argument("invalid signal parameters.");

        constexpr double PI = 3.14159265358979323846;
        std::vector<double> samples;

        for(std::size_t i = 0;i<sampleCount;i++){

            // time at which sample i is taken = sample no. / samplingFrequency  eg. for 0th sample -> i = 0, samplingFrequency = 1/5 => time = 0, i=1 time = 5, i=2 time = 10...
            double time = static_cast<double>(i)/samplingFrequency;
            double value = amplitude * std::sin(2.0*PI*frequency*time);
            samples.push_back(value);
       
        }

        return samples;    
    }
}