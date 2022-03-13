#pragma once

#include "order_book.hpp"

template <typename T>
void order_book::match(T& book, const fun::client::Price price, fun::client::Volume& volume, std::vector<fun::client::Trade>& trades)
{
    while (volume && book.can_match(price))
    {
        int match_order_ctr = 0;
        const auto& p_level = book.top_price_level();
        const int p_level_size = p_level.size();

        for (const auto& order : p_level)
        {
            if (order.volume > volume)
            {
                trades.push_back(fun::client::Trade{volume, static_cast<fun::client::Price>(book.top_price()), 0, order.ex_order_id, 0});
                break;
            }
            volume -= order.volume;
            ++match_order_ctr;
            trades.push_back(fun::client::Trade{order.volume, static_cast<fun::client::Price>(book.top_price()), 0, order.ex_order_id, 0});
            if (!volume) break;
        }

        if (match_order_ctr)
        {
            book.remove_top_orders(match_order_ctr);
        }

        if (volume && match_order_ctr < p_level_size)
        {
            book.reduce_top_order_qty_by(volume);
            volume = 0;
        }
    }
}
