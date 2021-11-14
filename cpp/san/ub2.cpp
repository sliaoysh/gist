#include <iostream>
using namespace std;

// clang++ -fsanitize=undefined -std=c++20 -m64 -O3 -Wall -Wextra -Wpedantic -o ub2 ub2.cpp

// No warning ......

struct ss
{
    int a;
    int b;
};

int main()
{
    char buf[8]{};
    buf[0] = 1;

    ss* p = (ss*)buf;
    cout << p->a << endl;
    cout << p->b << endl;

    return 0;
}
