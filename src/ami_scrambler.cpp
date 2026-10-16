#include "ami_scrambler.hpp"
#include "conventions.hpp"

namespace encodeco{
    // 8 0s -> 000VB0VB
Levels scrambleB8ZS (const Bits &bits){
    Levels levels;
    std::size_t len = bits.size();

    // Track polarity of last 1
    std::int8_t previousPulse = INITIAL_AMI_PREVIOUS_PULSE_POLARITY;
    std::size_t i=0;
    while(i<len){

        //first let's check whether there are 8 bits from the given index
        bool eightZeros = (i+7<bits.size());

        // then we'll check whether the next 8 bits are all 0s?

        if(eightZeros){
            for(std::size_t j=i;j<i+8;j++){
                if(bits[j]!=0)
                {
                    eightZeros=false;
                    break;
                }
            }
        }

        // if eightzeros is still true we have found a sequence of 8 zeros
        if(eightZeros){
            Levels replacement;

            if(previousPulse == POSITIVE_LEVEL){
                replacement = {0,0,0,POSITIVE_LEVEL,NEGATIVE_LEVEL,0,NEGATIVE_LEVEL,POSITIVE_LEVEL};
            }
            else{
                replacement = {0,0,0,NEGATIVE_LEVEL,POSITIVE_LEVEL,0,POSITIVE_LEVEL,NEGATIVE_LEVEL};
            }
            for(const auto level : replacement){
                levels.push_back(level);
                levels.push_back(level);
            }
            // set last pulse of the levels as previous pulse(for upcoming 0s)
            previousPulse = replacement.back();
            // increment by 8
            i+=8;
        }
        // no 8 consecutive zeros -> Normal AMI encoding
        else{
         // initialise to 0
        std::int8_t level = ZERO_LEVEL;
        if(bits[i]==1){
            level = -previousPulse;
            previousPulse = level;
        }
        levels.push_back(level);
        levels.push_back(level);
        i++;


        }

    }
    return levels;

}

Levels scrambleHDB3 (const Bits &bits){
    Levels levels;
    std::size_t len = bits.size();

    // Track polarity of last 1
    std::int8_t previousPulse = INITIAL_AMI_PREVIOUS_PULSE_POLARITY;
    std::size_t i=0;
       std::size_t nonZeroPulses = 0;
    while(i<len){

        //first let's check whether there are 4 bits from the given index
        bool fourZeros = (i+3<bits.size());

        // then we'll check whether the next 4 bits are all 0s?

        if(fourZeros){
            for(std::size_t j=i;j<i+4;j++){
                if(bits[j]!=0)
                {
                    fourZeros=false;
                    break;
                }
            }
        }


        // if fourzeros is still true we have found a sequence of 4 zeros starting from i
        if(fourZeros){
            Levels replacement;
          // if even non zero pulses since last substitution replace with BOOV
            if(nonZeroPulses%2==0)
            {
                std::int8_t B = -previousPulse;
                std::int8_t V = B;
                replacement = {B,0,0,V};
            }
            // odd non zero pules since last sub -> 000V
            else{
               std::int8_t V = previousPulse;
                replacement = {0,0,0,V};
            }
            // set last replacement to i+4(upto i+3 all are even)
          for(const auto level : replacement){
                levels.push_back(level);
                levels.push_back(level);
            }
            // set last pulse of the levels as previous pulse(for upcoming 0s)
            previousPulse = replacement.back();
            // reset the count of non zero pulses to 0 after substitution.
            nonZeroPulses = 0;
            // increment by 8
            i+=4;
        }
        // no 4 consecutive zeros -> Normal AMI encoding
        else{
         // initialise to 0
        std::int8_t level = ZERO_LEVEL;
        if(bits[i]==1){
            nonZeroPulses++;
            level = -previousPulse;
            previousPulse = level;
        }
        levels.push_back(level);
        levels.push_back(level);
        i++;


        }

    }
    return levels;

}
}
