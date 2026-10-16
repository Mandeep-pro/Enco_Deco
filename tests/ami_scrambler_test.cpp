
#include <cassert>
#include <iostream>

#include "ami_scrambler.hpp"

using namespace encodeco;

void testB8ZS() {
    assert(scrambleB8ZS({}).empty());

    // Ordinary AMI: +, 0, -
    assert((scrambleB8ZS({1, 0, 1}) == Levels{1, 1, 0, 0, -1, -1}));

    // Eight zeros assuming initial previous pulse is negative.
      assert(scrambleB8ZS({0, 0, 0, 0, 0, 0, 0, 0}) ==   Levels({0, 0, 0, 0, 0, 0, -1, -1, 1, 1, 0, 0, 1, 1, -1, -1}));

    // Previous pulse is positive.
    assert((scrambleB8ZS({1, 0, 0, 0, 0, 0, 0, 0, 0}) ==    Levels{1, 1, 0, 0, 0, 0, 0, 0, 1, 1,-1, -1, 0, 0, -1, -1, 1, 1}));

    // No eight-zero run.
    assert((scrambleB8ZS({1, 0, 1, 0, 1}) == Levels{1, 1, 0, 0, -1, -1, 0, 0, 1, 1}));

    std::cout << "B8ZS tests passed.\n";
}

void testHDB3() {
    assert(scrambleHDB3({}).empty());

    // 8 zeroes
    assert((scrambleHDB3({0, 0, 0, 0, 0, 0, 0, 0}) ==  Levels({1, 1, 0, 0, 0, 0, 1, 1,-1, -1, 0, 0, 0, 0, -1, -1})));
    // Ordinary AMI: +, 0, -
    assert((scrambleHDB3({1, 0, 1}) ==  Levels{1, 1, 0, 0, -1, -1}));

    // Even pulse count: B00V.
    assert((scrambleHDB3({0, 0, 0, 0}) == Levels{1, 1, 0, 0, 0, 0, 1, 1}));

    // Odd pulse count: 000V.
    assert((scrambleHDB3({1, 0, 0, 0, 0}) == Levels{1, 1, 0, 0, 0, 0, 0, 0, 1, 1}));

    // Two normal pulses, then B00V.
    assert((scrambleHDB3({1, 1, 0, 0, 0, 0}) == Levels{1, 1, -1, -1, 1, 1, 0, 0, 0, 0, 1, 1}));


    std::cout << "HDB3 tests passed.\n";
}

int main() {
    testB8ZS();
    testHDB3();

    std::cout << "All tests passed.\n";
    return 0;
}
