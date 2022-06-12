#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

int main()
{
    vector<int> v = {2,3,4,5,6,7,8,9};

    //int n = 0;
    //auto fn = [&n](const auto i) { return (i & 1) && (n++ == 0); };
    // the below lambda doesn't work ...
    auto fn = [n=0](const auto i) mutable { return (i & 1) && (n++ == 0); };

    auto it = remove_if(begin(v), end(v), fn);
    v.erase(it, end(v));

    for (const auto i : v)
    {
        cout << i << " ";
    }
    cout << endl;
    return 0;
}
