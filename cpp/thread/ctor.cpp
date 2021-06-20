#include <thread>
#include <iostream>
#include <functional>
using namespace std;

class c
{
public:
    c(){ cout << "default ctor" << endl; }
    c(const c&){ cout << "copy ctor" << endl; }
    c(c&&){ cout << "move ctor" << endl; }
};

void f1(const c&){}
void f2(c){}

int main()
{
    c cc1;
    thread t1{f1, cc1};
    t1.join();
    cout << "===========" << endl;

    thread t2{f1, c{}};
    t2.join();
    cout << "===========" << endl;

    c cc3;
    thread t3{f2, cc3};
    t3.join();
    cout << "===========" << endl;

    thread t4{f2, c{}};
    t4.join();
    cout << "===========" << endl;

    c cc5;
    thread t5{f1, ref(cc5)};
    t5.join();
    cout << "===========" << endl;

    return 0;
}
