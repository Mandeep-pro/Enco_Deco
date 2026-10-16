#include "line_encoder.hpp"
#include <cassert>
#include <iostream>

using namespace encodeco;
int main()
{
    

    Bits bits = {1, 0, 1,1,0,0,1,0};

    // Test NRZ-L.
    Levels result = encodeNRZL(bits);
    assert((result == Levels{1,1,-1,-1,1,1,1,1,-1,-1,-1,-1,1,1,-1,-1}));

    // Test NRZ-I, initial level +1.
    result = encodeNRZI(bits);
    assert((result == Levels{-1,-1,-1,-1,1,1,-1,-1,-1,-1,-1,-1,1,1,1,1}));

    // Test Manchester.
    result = encodeManchester(bits);
    assert((result == Levels{-1,1,1,-1,-1,1,-1,1,1,-1,1,-1,-1,1,1,-1}));

    // Test Differential Manchester, initial level +1.
    result = encodeDiffManchester(bits);
  assert((result == Levels{1,-1,1,-1,-1,1,1,-1,1,-1,1,-1,-1,1,-1,1}));

    // Test AMI, initial previous pulse polarity -1.
    result = encodeAMI(bits);
    assert((result == Levels{1,1,0,0,-1,-1,1,1,0,0,0,0,-1,-1,0,0}));

    // Test empty input. -> if you put in nothing you get nothing!
    Bits empty;
    assert(encodeNRZL(empty).empty());
    assert(encodeNRZI(empty).empty());
    assert(encodeManchester(empty).empty());
    assert(encodeDiffManchester(empty).empty());
    assert(encodeAMI(empty).empty());

    std::cout << "All line encoder tests passed.\n";
    return 0;
}