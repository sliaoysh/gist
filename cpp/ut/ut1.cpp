#include <iostream>
#include "ut3.hpp"
using namespace std;

int main()
{
    c c1{42};
    testing t1{c1};
    cout << t1.get() << endl;
    return 0;
}
