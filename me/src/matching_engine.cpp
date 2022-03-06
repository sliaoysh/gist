#include "matching_engine.hpp"

fun::client::Xoid matching_engine::add_order(const fun::client::InstrumentId instr_id, const fun::client::Side side, const fun::client::Price price, fun::client::Volume volume, std::vector<fun::client::Trade>& trades)
{
    const auto ex_order_id = ++ex_order_id_;
    order_books_[instr_id].add_order(ex_order_id, side, price, volume, trades);

    if (!trades.empty())
    {
        for (auto& trade : trades)
        {
            trade.counterOrderId = ex_order_id;
            trade.tradeId = ++trade_id_;
            auto it = order_details_.find(trade.orderId);

            if (trade.volume == it->second.volume)
            {
                order_details_.erase(it);
            }
            else
            {
                it->second.volume -= trade.volume;
            }
        }
    }
    if (volume)
    {
        order_details_[ex_order_id] = {price, volume, instr_id, side};
    }
    return ex_order_id;
}

bool matching_engine::cancel_order(const fun::client::Xoid ex_order_id)
{
    auto it = order_details_.find(ex_order_id);
    if (it == end(order_details_)) return false;
    const auto& order = it->second;
    order_books_[order.instrument_id].cancel_order(ex_order_id, order.side, order.price);
    order_details_.erase(it);
    return true;
}

matching_engine::matching_engine(const std::vector<fun::client::InstrumentId>& instruments)
{
    for (const auto instr_id : instruments)
        order_books_[instr_id] = order_book{};
}
