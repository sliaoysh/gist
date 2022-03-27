#include "order_book.hpp"

void order_book::match(fun::client::Side side, fun::client::Price price, fun::client::Volume& volume, std::vector<fun::client::Trade>& trades)
{
    if (side == fun::client::Side::buy)
    {
        match(a_book_, price, volume, trades);
    }
    else
    {
        match(b_book_, price, volume, trades);
    }
}

void order_book::add_order(const fun::client::Xoid ex_order_id, const fun::client::Side side, const fun::client::Price price, fun::client::Volume volume)
{
    if (side == fun::client::Side::buy)
    {
        b_book_.add_order(ex_order_id, price, volume);
    }
    else
    {
        a_book_.add_order(ex_order_id, price, volume);
    }
}

void order_book::cancel_order(const fun::client::Xoid ex_order_id, const fun::client::Side side, const fun::client::Price price)
{
    if (side == fun::client::Side::buy)
        b_book_.remove_order(ex_order_id, price);
    else
        a_book_.remove_order(ex_order_id, price);
}
