#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

void helper(string& str)
{
    for (auto head = begin(str); head != end(str);)
    {
        auto end_it = find_if_not(
                            head, end(str),
                            [=](const auto c)
                            {
                               return !!isalnum(*head) == !!isalnum(c);
                            });
        reverse(head, end_it);
        head = end_it;
    }
}

void rev_word(string& str)
{
    reverse(begin(str), end(str));
    helper(str);
}

int main()
{
    string str = "reverse words in this string";
    rev_word(str);
    cout << str << endl;
    return 0;
}

// ans : "string this in words reverse"
