#pragma once

#include "orders.hpp"

#include <algorithm>

template <fun::client::Side Side>
void orders<Side>::add_order(const fun::client::Xoid ex_order_id, const Price price, const fun::client::Volume volume)
{
    if (top_order_ctr_)
    {
        if (price_compare(price, top_order_price_))
        {
            const int i = price_index_above_top(price);

            if (i < 0)
            {
                const int mov_count = -i + init_top_index;
                const int ref_i = std::max(OB_ARRAY_SIZE-mov_count, top_index_);
                Price cur_p = price_mov_bottom(top_order_price_, ref_i-top_index_);

                for (int j = ref_i; j < OB_ARRAY_SIZE; ++j)
                {
                    if (!top_orders_[j].empty())
                    {
                        top_order_ctr_ -= top_orders_[j].size();
                        bottom_orders_[cur_p] = std::exchange(top_orders_[j], price_level{});
                    }
                    cur_p = price_mov_bottom(cur_p, 1);
                }

                if (top_order_ctr_)
                {
                    std::move_backward(next(begin(top_orders_),top_index_), next(begin(top_orders_),ref_i), next(begin(top_orders_),OB_ARRAY_SIZE));

                    if (ref_i != top_index_)
                    {
                        std::fill(next(begin(top_orders_),top_index_), next(begin(top_orders_),top_index_+mov_count), price_level{});
                    }
                }

                top_order_price_ = price;
                top_index_ = init_top_index;
                top_orders_[top_index_].push_back({ex_order_id, volume});
                ++top_order_ctr_;
            }
            else
            {
                top_orders_[i].push_back({ex_order_id, volume});
                top_index_ = i;
                top_order_price_ = price;
                ++top_order_ctr_;
            }
        }
        else
        {
            const auto end_array_price = price_mov_bottom(top_order_price_, OB_ARRAY_SIZE-top_index_);
            if (price_compare(price, end_array_price))
            {
                const int i = price_index(price);
                top_orders_[i].push_back({ex_order_id, volume});
                ++top_order_ctr_;
            }
            else
            {
                bottom_orders_[price].push_back({ex_order_id, volume});
                maintain_orderbook();
            }
        }
    }
    else
    {
        top_order_ctr_ = 1;
        top_order_price_ = price;
        top_orders_[top_index_].push_back(order_book_item{ex_order_id, volume});
    }
}

template <fun::client::Side Side>
void orders<Side>::remove_order(const fun::client::Xoid ex_order_id, const Price price)
{
    auto remove_fn = [ex_order_id](auto& p_level)
                     {
                        auto it = std::find_if(begin(p_level), end(p_level),
                                               [ex_order_id](const auto& item){ return item.ex_order_id == ex_order_id; });
                        p_level.erase(it);
                     };

    const auto end_array_price = price_mov_bottom(top_order_price_, OB_ARRAY_SIZE-top_index_);
    if (price_compare(price, end_array_price))
    {
        const int i = price_index(price);
        remove_fn(top_orders_[i]);
        --top_order_ctr_;
        maintain_orderbook();
    }
    else
    {
        auto it = bottom_orders_.find(price);
        remove_fn(it->second);
        if (it->second.empty())
        {
            bottom_orders_.erase(it);
        }
    }
}

template <fun::client::Side Side>
void orders<Side>::remove_top_orders(const int n)
{
    auto& p_level = top_orders_[top_index_];
    p_level.erase(begin(p_level), next(begin(p_level),n));
    top_order_ctr_ -= n;
    maintain_orderbook();
}

template <fun::client::Side Side>
void orders<Side>::maintain_orderbook()
{
    if (!top_order_ctr_)
    {
        top_index_ = init_top_index;
        if (!bottom_orders_.empty())
        {
            top_order_price_ = begin(bottom_orders_)->first;
            pull_orders_from_bottom(top_index_, top_order_price_);
        }
        return;
    }

    if (top_orders_[top_index_].empty())
    {
        const auto old_top_index = top_index_;
        while (top_orders_[++top_index_].empty());
        top_order_price_ = price_mov_bottom(top_order_price_, top_index_-old_top_index);
    }

    if (bottom_orders_.empty() ||
        top_index_ < LOW_ORDER_INDEX_THRESHOLD)
    {
        return;
    }

    const auto end_array_price = price_mov_bottom(top_order_price_, OB_ARRAY_SIZE-top_index_);
    const int diff = top_index_ - init_top_index;

    std::move(next(begin(top_orders_),top_index_), end(top_orders_), next(begin(top_orders_),init_top_index));
    const int i = OB_ARRAY_SIZE - diff;
    std::fill(next(begin(top_orders_),i), end(top_orders_), price_level{});
    top_index_ = init_top_index;
    pull_orders_from_bottom(i, end_array_price);
}

template <fun::client::Side Side>
void orders<Side>::pull_orders_from_bottom(const int index, const Price price)
{
    const auto end_array_price = price_mov_bottom(price, OB_ARRAY_SIZE-index);

    for (auto it = begin(bottom_orders_); it != end(bottom_orders_);)
    {
        if (!price_compare(it->first, end_array_price)) break;
        top_order_ctr_ += it->second.size();
        const auto diff = price_diff(price, it->first);
        top_orders_[index+diff] = move(it->second);
        it = bottom_orders_.erase(it);
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
    return top_order_ctr_ && !price_compare(price, top_order_price_);
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
