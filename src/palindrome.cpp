
#include "palindrome.hpp"
#include <algorithm>
#include <vector>

namespace encodeco
{
    Palindrome findLongestPalindrome(const Bits& bits)
    {
        const int n = static_cast<int>(bits.size());

        // if empty bitstream
        if (n == 0)return {0, 0};
 // initialise -> bestPalindrome starts at 0 and has length 1 
      Palindrome bestPalindrome = {0, 1};

        // Calculate odd-length palindrome radii.
        // oddRadius[i] includes the center bit.
        std::vector<int> oddRadius(n);

        int left = 0;
        int right = -1;

        for (int i = 0; i < n; i++)
        {
            int radius = 1;
            if (i <= right) radius = std::min(oddRadius[left + right - i], right - i + 1);
             // Expand while the bits on both sides match.
            while (radius <= i && i + radius < n && bits[i - radius] == bits[i + radius]) radius++;
   
            oddRadius[i] = radius;
            const int start = i - radius + 1;
            const int length = 2 * radius - 1;

            // Update only if this palindrome is longer.
            // Equal lengths keep the first palindrome found.
            if (length > static_cast<int>(bestPalindrome.length))
            {
                bestPalindrome = { static_cast<std::size_t>(start),static_cast<std::size_t>(length)};
            }

            // Update the rightmost palindrome boundaries.
            if (i + radius - 1 > right)
            {
                left = i - radius + 1;
                right = i + radius - 1;
            }
        }

        // Calculate even-length palindrome radii.
        // evenRadius[i] is centered between bits i-1 and i.
        std::vector<int> evenRadius(n);

        left = 0;
        right = -1;

        for (int i = 0; i < n; i++)
        {
            int radius = 0;

            if (i <= right) radius = std::min(evenRadius[left+right-i+1], right-i+1);
          // Expand while the bits on both sides match.
            while (i -radius-1 >= 0 && i + radius < n && bits[i-radius-1] == bits[i + radius]) radius++;
         
           evenRadius[i] = radius;
            const int start = i - radius;
            const int length = 2 * radius;

            // Update only if this palindrome is longer.
            // Equal lengths keep the first palindrome found.
            if (length > static_cast<int>(bestPalindrome.length))
            {
                bestPalindrome = {static_cast<std::size_t>(start),  static_cast<std::size_t>(length)};
            }
             // Update the rightmost palindrome boundaries.
            if (i + radius - 1 > right)
            {
                left = i - radius;
                right = i + radius - 1;
            }
        }

        return bestPalindrome;
    }
}
