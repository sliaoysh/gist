#include <iostream>
#include "a/adder.h"
using namespace std;

int main()
{
    const auto r = add(1, 2);
    cout << r << endl;
    return 0;
}
