
#include "palindrome.hpp"

#include <cassert>
#include <iostream>
 using namespace encodeco;
int main()
{
   

    // Test 1: Odd-length palindrome.
    // Bits odd = {1, 0, 1, 1, 1, 0};
    // Palindrome result = findLongestPalindrome(odd);
    // assert(result.start == 0);
    // assert(result.length == 3);

    // Test 2: Even-length palindrome.
    Bits even = {1, 0, 0, 1, 1, 0};
    Palindrome result  = findLongestPalindrome(even);
    assert(result.start == 0);
    assert(result.length == 4);

    // Test 3: All bits are identical.
    Bits identical = {1, 1, 1, 1};
    result = findLongestPalindrome(identical);
    assert(result.start == 0);
    assert(result.length == 4);

    // Test 4: Empty bitstream.
    Bits empty = {};
    result = findLongestPalindrome(empty);
    assert(result.start == 0);
    assert(result.length == 0);

    // Test 5: Single bit.
    Bits single = {1};
    result = findLongestPalindrome(single);
    assert(result.start == 0);
    assert(result.length == 1);

    // Test 6: Two equal-length longest palindromes.
    // The earliest one must be returned.
    Bits tiedEven = {1, 0, 0, 1, 1, 0, 1};
    result = findLongestPalindrome(tiedEven);
    assert(result.start == 0);
    assert(result.length == 4);

    std::cout << "All palindrome tests passed.\n";

    return 0;
}
