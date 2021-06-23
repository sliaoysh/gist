#include <iostream>
#include <coroutine>
using namespace std;

struct ReturnObject
{
    struct promise_type
    {
        promise_type() = default;
        ReturnObject get_return_object(){ return {}; }
        suspend_never initial_suspend(){ return {}; }
        suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception(){}
    };
};

ReturnObject foo()
{
    cout << "111" << endl;

    //co_await suspend_always{};
    co_await suspend_never{};

    cout << "222" << endl;
}

int main()
{
    foo();

    return 0;
}
