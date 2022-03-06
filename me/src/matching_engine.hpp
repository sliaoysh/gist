#pragma once

#include "order_book.hpp"
#include "order_detail.hpp"

#include <unordered_map>

class matching_engine
{
public:
    matching_engine(const std::vector<fun::client::InstrumentId>& instruments);

    fun::client::Xoid add_order(fun::client::InstrumentId instr_id, fun::client::Side side, fun::client::Price price, fun::client::Volume volume, std::vector<fun::client::Trade>& trades);
    bool cancel_order(fun::client::Xoid ex_order_id);

private:
    std::unordered_map<fun::client::InstrumentId,order_book> order_books_;
    std::unordered_map<fun::client::Xoid,order_detail> order_details_;
    fun::client::Xoid ex_order_id_ = 0;
    fun::client::TradeId trade_id_ = 0;
};
