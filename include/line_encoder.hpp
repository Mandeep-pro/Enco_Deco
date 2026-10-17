#ifndef ENCO_DECO_LINE_ENCODER_HPP
#define ENCO_DECO_LINE_ENCODER_HPP

#include "types.hpp"

namespace encodeco{
    Levels encodeNRZL(const Bits& bits);
    Levels encodeNRZI(const Bits& bits);
    Levels encodeManchester(const Bits& bits);
    Levels encodeDiffManchester(const Bits& bits);
     Levels encodeAMI(const Bits& bits);
}



#endif