#include <iostream>
using namespace std;

class c
{
public:
    c(){ cout << "default ctor" << endl; }
    c(const c&){ cout << "copy ctor" << endl; }
    c(c&&){ cout << "move ctor" << endl; }
};

c f1(bool b)
{
    if (b)
    {
        c cc1;
        cout << "then..." << endl;
        return cc1;
    }
    else
    {
        const c cc2;
        cout << "else..." << endl;
        return cc2;
    }
}

int main()
{
    auto c1 = f1(true);
    cout << "========" << endl;
    auto c2 = f1(false);
    (void)c1;
    (void)c2;
    return 0;
}
