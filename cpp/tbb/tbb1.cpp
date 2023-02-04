#include <iostream>
#include <vector>
#include <execution>
using namespace std;

int main()
{
    vector v1{1,2,3,4,5,6,7,8,9};
    for_each(execution::par_unseq,
             begin(v1), end(v1),
             [](auto x){ cout << x; });

    cout << endl;
}
