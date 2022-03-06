#include <iostream>
using namespace std;

//  clang++ -fsanitize=address -std=c++17 -m64 -O0 -g -Wall -Wextra -Wpedantic -o addr addr.cpp

int main()
{
    int* p = new int{42};
    cout << *p << endl;

    delete p;
    cout << *p << endl;

    return 0;
}
