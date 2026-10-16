#ifndef ENCO_DECO_AMI_SCRAMBLER_HPP
#define ENCO_DECO_AMI_SCRAMBLER_HPP

#include "types.hpp"

namespace encodeco{
    Levels scrambleHDB3(const Bits &bits);
    Levels scrambleB8ZS(const Bits &bits);
}



#endif