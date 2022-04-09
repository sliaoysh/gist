#pragma once

#include "order_book_item.hpp"

#include <deque>
#include <functional>
#include <map>
#include <type_traits>
#include <vector>

using Price = int64_t;

template <fun::client::Side Side>
class orders
{
public:
    using price_level = std::vector<order_book_item>;
    using price_mov_bottom_op = std::conditional_t<Side==fun::client::Side::buy, std::minus<Price>, std::plus<Price>>;
    using price_compare_pred = std::conditional_t<Side==fun::client::Side::buy, std::greater<Price>, std::less<Price>>;

    static constexpr int OB_ARRAY_SIZE = 2048;
    static constexpr int init_top_index = 1024;

    static constexpr price_mov_bottom_op price_mov_bottom{};
    static constexpr price_compare_pred price_compare{};

    orders();

    void add_order(fun::client::Xoid ex_order_id, Price price, fun::client::Volume volume);
    void remove_order(fun::client::Xoid ex_order_id, Price price);
    void remove_top_orders(int n);
    void reduce_top_order_qty_by(fun::client::Volume vol);

    bool can_match(Price price) const;
    Price top_price() const;
    const price_level& top_price_level() const;

private:
    void update_top_index();

    int price_index(Price price) const;
    int price_index_above_top(Price price) const;

    std::deque<price_level> top_orders_;

    Price top_order_price_ = -1;
    int top_index_ = init_top_index;
    int ctr_ = 0;
    int book_size_ = OB_ARRAY_SIZE;
};

#include "orders.ipp"
