#include <iostream>
#include <utility>
using namespace std;

template <auto N>
constexpr auto loop =
    [](auto&& expr)
    {
        [expr]<auto... Is>(index_sequence<Is...>)
        {
            ((expr(), (void)Is), ...);
        }(make_index_sequence<N>{});
    };

int main()
{
    loop<3>([](){ cout << "expr" << endl; });
    return 0;
}
