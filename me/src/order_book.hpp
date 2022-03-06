#pragma once

#include "message_types.hpp"
#include "orders.hpp"

class order_book
{
public:
    void add_order(fun::client::Xoid ex_order_id, fun::client::Side side, fun::client::Price price, fun::client::Volume& volume, std::vector<fun::client::Trade>& trades);
    void cancel_order(fun::client::Xoid ex_order_id, fun::client::Side side, fun::client::Price price);

private:
    template <typename T>
    void match(T& book, fun::client::Price price, fun::client::Volume& volume, std::vector<fun::client::Trade>& trades);

    orders<fun::client::Side::buy> b_book_;
    orders<fun::client::Side::sell> a_book_;
};

#include "order_book.ipp"
