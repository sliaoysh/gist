#include <limits>

// clang++ -fsanitize=undefined -std=c++20 -m64 -O3 -Wall -Wextra -Wpedantic -o ub3 ub3.cpp

int main()
{
    unsigned int x1=std::numeric_limits<unsigned int>::max()+1;
    unsigned int x2=0u-1u;
    int y1=std::numeric_limits<int>::max()+1;
    int y2=std::numeric_limits<int>::min()-1;

    return 0;
}
