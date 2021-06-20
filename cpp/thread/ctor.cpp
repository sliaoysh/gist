#include <thread>
#include <iostream>
using namespace std;

class c
{
public:
    c(){ cout << "default ctor" << endl; }
    c(const c&){ cout << "copy ctor" << endl; }
    c(c&&){ cout << "move ctor" << endl; }
};

void f1(const c& cc){}
void f2(c cc){}

int main()
{
    c cc1;
    thread t1{f1, cc1};
    t1.join();
    cout << "===========" << endl;

    thread t2{f1, {}};
    t2.join();
    cout << "===========" << endl;

    c cc3;
    thread t3{f2, cc3};
    t3.join();
    cout << "===========" << endl;

    thread t4{f2, {}};
    t4.join();
    cout << "===========" << endl;

    return 0;
}
