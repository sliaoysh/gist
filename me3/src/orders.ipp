#pragma once

#include "orders.hpp"

#include <algorithm>

template <fun::client::Side Side>
void orders<Side>::add_order(const fun::client::Xoid ex_order_id, const Price price, const fun::client::Volume volume)
{
    if (top_order_price_ >= 0)
    {
        if (price_compare(price, top_order_price_))
        {
            int i = price_index_above_top(price);
            if (i < 0)
            {
                top_orders_.insert(begin(top_orders_), -i, {});
                book_size_ += -i;
                i = 0;
            }
            top_orders_[i].push_back({ex_order_id, volume});
            top_index_ = i;
            top_order_price_ = price;
        }
        else
        {
            const int i = price_index(price);
            if (i >= book_size_)
            {
                const int n = i - book_size_ + 1;
                book_size_ += n;
                top_orders_.insert(end(top_orders_), n, {});
            }
            top_orders_[i].push_back({ex_order_id, volume});
        }
    }
    else
    {
        top_order_price_ = price;
        top_orders_[top_index_].push_back(order_book_item{ex_order_id, volume});
    }
    ++ctr_;
}

template <fun::client::Side Side>
void orders<Side>::remove_order(const fun::client::Xoid ex_order_id, const Price price)
{
    const int i = price_index(price);
    auto it = std::find_if(begin(top_orders_[i]), end(top_orders_[i]),
                                 [ex_order_id](const auto& item){ return item.ex_order_id == ex_order_id; });
    top_orders_[i].erase(it);
    --ctr_;
    if (i == top_index_)
    {
        update_top_index();
    }
}

template <fun::client::Side Side>
void orders<Side>::remove_top_orders(const int n)
{
    auto& p_level = top_orders_[top_index_];
    p_level.erase(begin(p_level), next(begin(p_level),n));
    ctr_ -= n;
    update_top_index();
}

template <fun::client::Side Side>
void orders<Side>::update_top_index()
{
    if (!ctr_)
    {
        top_order_price_ = -1;
        top_index_ = init_top_index;
        return;
    }

    while (top_orders_[top_index_].empty())
    {
        ++top_index_;
        top_order_price_ = price_mov_bottom(top_order_price_, 1);
    }
}

template <fun::client::Side Side>
void orders<Side>::reduce_top_order_qty_by(const fun::client::Volume vol)
{
    top_orders_[top_index_][0].volume -= vol;
}

template <fun::client::Side Side>
int orders<Side>::price_index(const Price price) const
{
    if constexpr (Side == fun::client::Side::buy)
    {
        return top_index_ + static_cast<int>(top_order_price_ - price);
    }
    else
    {
        return top_index_ + static_cast<int>(price - top_order_price_);
    }
}

template <fun::client::Side Side>
int orders<Side>::price_diff(const Price top_price, const Price bottom_price) const
{
    if constexpr (Side == fun::client::Side::buy)
    {
        return top_price - bottom_price;
    }
    else
    {
        return bottom_price - top_price;
    }
}

template <fun::client::Side Side>
int orders<Side>::price_index_above_top(const Price price) const
{
    if constexpr (Side == fun::client::Side::buy)
    {
        return top_index_ - static_cast<int>(price - top_order_price_);
    }
    else
    {
        return top_index_ - static_cast<int>(top_order_price_ - price);
    }
}

template <fun::client::Side Side>
bool orders<Side>::can_match(const Price price) const
{
    return top_order_price_ >= 0 && !price_compare(price, top_order_price_);
}

template <fun::client::Side Side>
Price orders<Side>::top_price() const
{
    return top_order_price_;
}

template <fun::client::Side Side>
const typename orders<Side>::price_level& orders<Side>::top_price_level() const
{
    return top_orders_[top_index_];
}

template <fun::client::Side Side>
orders<Side>::orders()
    : top_orders_(OB_ARRAY_SIZE)
{
    for (auto& v : top_orders_)
    {
        v.reserve(800);
    }
}
