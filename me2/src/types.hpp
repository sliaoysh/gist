/*
 * Copyright (C) 2022
 */

#pragma once

#include <cstdint>

namespace fun::client {

// template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
// template<class... Ts> overload(Ts...) -> overload<Ts...>; // line not needed in

using Xoid = uint64_t;
const Xoid nullXoid = 0;

// just for feedplayer to identify the order before having the xoid from ME
using Coid = uint64_t;
const Coid nullCoid = 0;

using InstrumentId = uint32_t;
using Price = uint64_t;
using Volume = uint64_t;
using Byte = uint8_t;
using TradeId = uint64_t;
const TradeId nullTradeId = 0;
using Time = uint64_t;

enum class OrderType : Byte {
    gtc = 'G',
};

enum class Side : Byte {
    buy = 'B',
    sell = 'S',
};

}  // namespace fun::client
