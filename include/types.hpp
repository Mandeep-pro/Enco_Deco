/*
Given cpp header file defines all the data types used in the assignment
*/
// preprocessing instructions
#ifndef ENCO_DECO_TYPES_HPP /* Header guards to avoid processing a header file again and again in same cpp file(some other header files may call it uno )*/
#define ENCO_DECO_TYPES_HPP
/*sTUPID BUG -> 0 LIKH DIYA THA deco MAI - TOOK 5 MINUTES TO NOTICE*/

#include <cstddef>  // for size_t
#include <cstdint> // for fixed width int data types uint8_t and int8_t
#include <vector>

/*Namespace for the assignment to prevent any accidental naming conflict with any other namespace object */
namespace encodeco {
    // Aliases for vectors of unsigned nos. using 8 bits and vectors of signed nos. using 8 bits
    using Bits = std::vector<std::uint8_t>; // for sequences of 0s and 1s
    using Levels = std::vector<std::int8_t>;

    struct Palindrome{
        std::size_t start; 
        std::size_t length;
    };

    struct RGB{
        std::uint8_t r,g,b;
    };
}

/*size_t is unsigned int -> whatever the architecture supports - 64 bits? 32 bit? */


#endif

