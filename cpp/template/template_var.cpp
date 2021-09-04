#include <iostream>
using namespace std;

template <int N>
constexpr long long X = 2 * X<N-1>;

template <>
constexpr long long X<0> = 1;

template <int N>
constexpr long long f()
{
    if constexpr (N == 0) return 1;
    else return 2 * f<N-1>();
}

int main()
{
    static_assert(X<10> == 1024);
    static_assert(f<10>() == 1024);
    cout << "end" << endl;
    return 0;
}
