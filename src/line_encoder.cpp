#include "line_encoder.hpp"
#include "conventions.hpp"

namespace encodeco{
 Levels encodeNRZL (const  Bits &bits){
    // levels is a vector containing 1 byte signed ints.
    Levels levels;
  /*
  If bits is 101 , levels becomes +1,+1,-1,-1,+1,+1.
  */
    for(const auto bit : bits){
        const auto level = (bit==1) ? NRZ_L_ONE_LEVEL : NRZ_L_ZERO_LEVEL;
        levels.push_back(level);
        levels.push_back(level);
    }
    return levels;
 }

 Levels encodeNRZI(const Bits &bits){
    Levels levels;
  
    std::int8_t previousLevel =  INITIAL_NRZ_I_LEVEL;
    for(const auto bit : bits){
        std::int8_t level{0};
        // first 1 is -1...
        if(bit == 1){
            previousLevel = -previousLevel;
            level = previousLevel;
        }
        else level = previousLevel;
        levels.push_back(level);
        levels.push_back(level);
    }
    return levels;

 }

 Levels encodeManchester(const Bits &bits){
    Levels levels;
    for(const auto bit : bits){
        const auto level_first_half = (bit == 1) ? MANCHESTER_ONE_FIRST_HALF : MANCHESTER_ZERO_FIRST_HALF;
        const auto level_second_half = (bit == 1) ? MANCHESTER_ONE_SECOND_HALF : MANCHESTER_ZERO_SECOND_HALF;
        levels.push_back(level_first_half);
        levels.push_back(level_second_half);
    }
    return levels;
 }

 Levels encodeDiffManchester(const Bits &bits){
    Levels levels;
    std::int8_t previous_second_half_level = INITIAL_DIFF_MANCHESTER_LEVEL; // which is +1
    for(const auto bit : bits){
        std::int8_t level_first_half = 0;
        std::int8_t level_second_half = 0;
      if(bit==0){
        // transition for zero
        level_first_half = -previous_second_half_level;
      }
      else{
        level_first_half = previous_second_half_level;
      }
        level_second_half =-level_first_half;
        levels.push_back(level_first_half);
        levels.push_back(level_second_half);
        previous_second_half_level = level_second_half;
    }
    return levels;
 }
// non scrambled ami
 Levels encodeAMI(const Bits &bits){
    Levels levels;
    std::int8_t previous_one_polarity = INITIAL_AMI_PREVIOUS_PULSE_POLARITY;
    for(const auto bit: bits){
        std::int8_t level = 0;
        if(bit==1){
            level = -previous_one_polarity;
            previous_one_polarity = level;
        }
        levels.push_back(level);
        levels.push_back(level);
    }
    return levels;
 }


}

