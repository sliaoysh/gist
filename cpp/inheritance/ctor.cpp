#include <iostream>
using namespace std;

class base
{
public:
    base(){ cout << "base default ctor" << endl; }
    base(const base&){ cout << "base copy ctor" << endl; }
    base(base&&){ cout << "base move ctor" << endl; }
};

class derived : public base
{
public:
    derived(){ cout << "derived default ctor" << endl; }
    derived(const derived& d): base{d}{ cout << "derived copy ctor" << endl; }
    derived(derived&& d): base{move(d)}{ cout << "derived move ctor" << endl; }
};

int main()
{
    derived d1;
    cout << "==========" << endl;
    derived d2 = d1;
    cout << "==========" << endl;
    derived d3 = move(d2);
    cout << "==========" << endl;
    return 0;
}
