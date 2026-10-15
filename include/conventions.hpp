#ifndef ENCO_DECO_CONVENTIONS_HPP
#define ENCO_DECO_CONVENTIONS_HPP

/*defines variables for various encodings*/
#include <cstdint>
namespace encodeco
{

using SignalLevel = std::int8_t;


/*Usual!*/
constexpr SignalLevel POSITIVE_LEVEL = 1;
constexpr SignalLevel ZERO_LEVEL = 0;
constexpr SignalLevel NEGATIVE_LEVEL = -1;

/*NRZ-L 1 means POSITIVE_LEVEL 0 means NEGATIVE_LEVEL*/
constexpr SignalLevel NRZ_L_ONE_LEVEL = POSITIVE_LEVEL;
constexpr SignalLevel NRZ_L_ZERO_LEVEL = NEGATIVE_LEVEL;

// NRZ-I 0 = no change. 1 = inversion
constexpr SignalLevel INITIAL_NRZ_I_LEVEL = POSITIVE_LEVEL;

// Manchester : 1 = low to high , 0 is high to low(IEEE 802.3)
constexpr SignalLevel MANCHESTER_ONE_FIRST_HALF = NEGATIVE_LEVEL;
constexpr SignalLevel MANCHESTER_ONE_SECOND_HALF = POSITIVE_LEVEL;
constexpr SignalLevel MANCHESTER_ZERO_FIRST_HALF = POSITIVE_LEVEL;
constexpr SignalLevel MANCHESTER_ZERO_SECOND_HALF = NEGATIVE_LEVEL;

/*Diff Manchester -> transition at start for 0*/
constexpr SignalLevel INITIAL_DIFF_MANCHESTER_LEVEL = POSITIVE_LEVEL;

/*AMI: first 1 is +ve(assumption) so previous ami polarity is -1. this will keep switching!*/
constexpr SignalLevel INITIAL_AMI_PREVIOUS_PULSE_POLARITY = NEGATIVE_LEVEL;
}


#endif