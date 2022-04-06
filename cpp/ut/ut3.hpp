#include "ut2.hpp"

class testing
{
public:
    testing(c& cc): cc_{cc}{}
    int get(){ return cc_.x_; }
private:
    c& cc_;
};
