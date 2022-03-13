#include "matching_engine.hpp"

#include <iostream>

fun::client::Xoid matching_engine::match(fun::client::InstrumentId instr_id, fun::client::Side side, fun::client::Price price, fun::client::Volume& volume, std::vector<fun::client::Trade>& trades)
{
    const auto ex_order_id = ++ex_order_id_;
    ob_ = &order_books_[instr_id];
    ob_->match(side, price, volume, trades);

    if (!trades.empty())
    {
        for (auto& trade : trades)
        {
            trade.counterOrderId = ex_order_id;
            trade.tradeId = ++trade_id_;

            if (trade.volume == order_details_[trade.orderId].volume)
            {
                order_details_[trade.orderId].instrument_id = 0;
            }
            else
            {
                order_details_[trade.orderId].volume -= trade.volume;
            }
        }
    }
    return ex_order_id;
}

void matching_engine::add_order(const fun::client::Xoid ex_order_id, fun::client::InstrumentId instr_id, const fun::client::Side side, const fun::client::Price price, fun::client::Volume volume)
{
    ob_->add_order(ex_order_id, side, price, volume);
    order_details_[ex_order_id] = {price, volume, instr_id, side};
}

bool matching_engine::cancel_order_check(const fun::client::Xoid ex_order_id) const
{
    return order_details_[ex_order_id].instrument_id != 0;
}

void matching_engine::cancel_order(const fun::client::Xoid ex_order_id)
{
    auto& order = order_details_[ex_order_id];
    order_books_[order.instrument_id].cancel_order(ex_order_id, order.side, order.price);
    order.instrument_id = 0;
}

matching_engine::matching_engine(const std::vector<fun::client::InstrumentId>& instruments)
    : order_details_(1000000, order_detail{})
{
    for (const auto instr_id : instruments)
        order_books_[instr_id] = order_book{};

    std::cout << order_books_.size() << std::endl;
}
