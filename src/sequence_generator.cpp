#include "sequence_generator.hpp" // already includes types.hpp
#include <random>
#include <stdexcept>

namespace encodeco
{
    // for n length O(n) time and space.
    Bits generateRandomBits(std::size_t length)
    {
        // pseudo random no. generator (ints) gets reused everytime called once initialised!
        // static persists for the whole program.
        // random_device gives the seed (same seed with 2 same generators -> same numbers generated)
        // to initialise the generator.
        static std::mt19937 generator(std::random_device{}());

        // distribution takes values produced by generator and maps to 0 or 1 (with equal probability)
        static std::uniform_int_distribution<int> distribution(0, 1);

        Bits bits(length);

        for (std::size_t index = 0; index < length; index++)
        {
            // here distribution takes a number from generator and maps to 0 or 1 (ints)
            // can typecast to uint8_t (type of Bits[index])
            bits[index] = static_cast<std::uint8_t>(distribution(generator));
        }

        return bits;
    }

    Bits generateBitsWithZeroRuns(
        std::size_t length,
        std::size_t zeroRunLength,
        std::size_t runCount)
    {
        

        // The assignment requires zero-runs of either 4 or 8 bits.
        if (zeroRunLength != 4 && zeroRunLength != 8)
        {
            throw std::invalid_argument(
                "Zero-run length must be either 4 or 8.");
        }
            // Generate the initial random bitstream.
        Bits bits = generateRandomBits(length);
        // No zero runs
        if (runCount == 0)
        {
            return bits;
        }

        // Check whether the bitstream can accommodate all the requested runs.
        if (runCount > length / zeroRunLength)
        {
            throw std::invalid_argument(
                "Bitstream is too short for the requested zero-runs.");
        }

        // Divide the bitstream into divisions, one for each zero-run. they could have more elements than the length of the run (multiple positions to start the run)
        const std::size_t divisionLength = length / runCount;
            // Choose a random starting position within this division.
            static std::mt19937 generator(std::random_device{}());
        for (std::size_t runIndex = 0; runIndex < runCount; runIndex++)
        {   
            
            const std::size_t divisionStart = runIndex * divisionLength;
            // if last run division ends at length(end of the bit stream), else at divisionsStart + divisionLength. both are exclusive.
            const std::size_t divisionEnd = (runIndex == runCount - 1)? length : divisionStart + divisionLength;

            // The latest position at which we can fit a complete zero-run within this division
            const std::size_t latestStart = divisionEnd - zeroRunLength;

           

            std::uniform_int_distribution<std::size_t> distribution(
                divisionStart, latestStart);
                // choose between divisionStart (right from the beginning of division) or latestStart(last possible candidate for start) as the place where the zero run start

            const std::size_t runStart = distribution(generator); // chooses for each division where the run will start

            // Plant the zero-run by setting consecutive bits to zero.
            for (std::size_t index = runStart;index < runStart + zeroRunLength; index++)
            {
                bits[index] = 0;
            }
        }

        return bits;
    }
}