
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>

#include "sequence_generator.hpp"

using namespace encodeco;

int main()
{
    // Test 1: Generate random bits.
    Bits bits = generateRandomBits(100);

    assert(bits.size() == 100);
   
    for (std::uint8_t bit : bits)
    {
      
        assert(bit == 0 || bit == 1);
       
    }
     std::cout << "Sequence of 100 bits: ";
     for(auto it: bits){
        std::cout << static_cast<int>(it) << " ";
    }
    std::cout << "\n";    
    std::cout << "Test 1 passed: Random bits are valid.\n";

    // Test 2: Generate bits with planted zero-runs.
    Bits bitsWithRuns = generateBitsWithZeroRuns(100, 4, 3);

    assert(bitsWithRuns.size() == 100);

    for (std::uint8_t bit : bitsWithRuns)
    {
        assert(bit == 0 || bit == 1);
    }

    // Each division must contain at least one run of four zeros.
    const std::size_t divisionLength = 100 / 3;

    for (std::size_t runIndex = 0; runIndex < 3; ++runIndex)
    {
        const std::size_t divisionStart = runIndex * divisionLength;
        const std::size_t divisionEnd = (runIndex == 2) ? 100 : divisionStart + divisionLength;

        bool foundRun = false;

        for (std::size_t start = divisionStart; start + 4 <= divisionEnd;    ++start)
        {
            bool allZeros = true;

            for (std::size_t index = start; index < start + 4; ++index)
            {
                if (bitsWithRuns[index] != 0)
                {
                    allZeros = false;
                    break;
                }
            }

            if (allZeros)
            {
                foundRun = true;
                break;
            }
        }

        assert(foundRun);
    }

    std::cout << "Test 2 passed: Each division contains four consecutive zeros.\n";

    // Test 3: Reject an invalid zero-run length.
    bool invalidLengthRejected = false;

    try
    {
        generateBitsWithZeroRuns(100, 5, 3);
    }
    catch (const std::invalid_argument&)
    {
        invalidLengthRejected = true;
    }

    assert(invalidLengthRejected);

    std::cout << "Test 3 passed: Invalid zero-run length rejected.\n";

    // Test 4: Reject an impossible number of zero-runs.
    bool impossibleRunsRejected = false;

    try
    {
        generateBitsWithZeroRuns(10, 8, 2);
    }
    catch (const std::invalid_argument&)
    {
        impossibleRunsRejected = true;
    }

    assert(impossibleRunsRejected);

    std::cout << "Test 4 passed: Impossible run count rejected.\n";

    std::cout << "All tests passed!\n";

    return 0;
}
