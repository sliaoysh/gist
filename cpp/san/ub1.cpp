#include <string>
#include <iostream>
using namespace std;

// clang++ -fsanitize=undefined -std=c++17 -m64 -O3 -Wall -Wextra -Wpedantic -o ub1 ub1.cpp

int f(int i)
{
    string ss;

    i = i << 40;

    return ss[i];
}

int main()
{
    int x = f(10);

    cout << x << endl;

    return 0;
}
