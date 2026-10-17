#include "delta_modulation.hpp"
#include <stdexcept>

namespace encodeco{
    Bits encodeDeltaModulation(const std::vector<double> &samples, double stepSize, double initialValue){
        if(stepSize <= 0.0){
            throw std::invalid_argument("Step size must be positive.");
        }
        Bits bits;
        double staircase = initialValue;

        for(double sample:samples){
            if(sample>staircase){
                bits.push_back(1);
                staircase += stepSize;
            }
            else{
                bits.push_back(0);
                staircase -= stepSize;
            }
        }
        return bits;
    }

     std::vector<double> decodeDeltaModulation(const Bits &bits, double stepSize, double initialValue){
        if(stepSize <= 0.0) throw std::invalid_argument("Step size must be positive.");

        std::vector<double> reconstructed;

        double staircase = initialValue;

        // if 1 add stepSize to staircase, if 0 subtract stepSize to staircase. new staircse is sample value.
        for(auto bit: bits){
            if(bit == 1) staircase += stepSize;
            else if(bit == 0) staircase -= stepSize;
            else throw std::invalid_argument("Bits must be 0/1");
            reconstructed.push_back(staircase);
        }
      return  reconstructed;
        
     }


}