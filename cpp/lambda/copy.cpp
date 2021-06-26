#include <iostream>
#include <memory>
#include <string>
using namespace std;

int main()
{
    string str = "abc";

    auto f1 = [str](auto c) mutable { cout << c << ", " << str << endl; };
    auto f2 = f1;
    f2('1');
    cout << "=============" << endl;

    auto f3 = [&str](auto c) mutable { cout << c << ", " << str << endl; };
    auto f4 = f3;
    f4('2');
    cout << "=============" << endl;

    auto f5 = [p=make_unique<string>()](auto c){ cout << c << ", " << *p << endl; };
    // auto f6 = f5; // error
    f5('3');

    auto f7 = [p=make_unique<string>()](auto c){ cout << c << ", " << *p << endl; };
    auto f8 = move(f7);
    f8('4');

    return 0;
}
