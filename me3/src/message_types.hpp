/*
 * Copyright (C) 2022
 */

#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <variant>
#include <sstream>

#include "types.hpp"

namespace fun::client {

using Receiving = std::function<ssize_t(Byte *, size_t)>;
using Sending = std::function<ssize_t(const Byte *, size_t)>;

enum class MessageType : Byte {
    addOrder = 'A',
    cancelOrder = 'D',
    addOrderAccepted = 'Y',
    cancelOrderAccepted = 'C',
    cancelOrderRejected = 'J',
    trade = 'T',
};

#pragma pack(push, 1)
struct MessageHeader {
    MessageHeader(size_t wholeSize, MessageType type) : size(wholeSize - sizeof(MessageHeader::size)), msgType(type) {}
    uint8_t size;
    MessageType msgType;
};

struct AddOrder : MessageHeader {
    AddOrder(Side side_, OrderType orderType_, InstrumentId instrumentId_, Volume volume_, Price price_)
        : MessageHeader{sizeof(AddOrder), MessageType::addOrder},
          side(side_),
          orderType(orderType_),
          instrumentId(instrumentId_),
          volume(volume_),
          price(price_) {}

    Side side;
    OrderType orderType;
    InstrumentId instrumentId;
    Volume volume;
    Price price;
    std::string string() {
        std::stringstream s;
        s << "type=AddOrder, side=" << char(side) << ", orderType=" << char(orderType)
          << ", instrumentId=" << instrumentId << ", volume=" << volume << ", price=" << price;
        return s.str();
    }
};

struct CancelOrder : MessageHeader {
    CancelOrder(Xoid exchangeOrderId_)
        : MessageHeader{sizeof(CancelOrder), MessageType::cancelOrder}, exchangeOrderId(exchangeOrderId_) {}
    Xoid exchangeOrderId;
    std::string string() {
        std::stringstream s;
        s << "type=CancelOrder, xoid=" << exchangeOrderId;
        return s.str();
    }
};

struct AddOrderAccepted : MessageHeader {
    AddOrderAccepted(Xoid exchangeOrderId_)
        : MessageHeader{sizeof(AddOrderAccepted), MessageType::addOrderAccepted}, exchangeOrderId(exchangeOrderId_) {}
    Xoid exchangeOrderId;
    std::string string() {
        std::stringstream s;
        s << "type=AddOrderAccepted, xoid=" << exchangeOrderId;
        return s.str();
    }
};

struct CancelOrderAccepted : MessageHeader {
    CancelOrderAccepted(Xoid exchangeOrderId_)
        : MessageHeader{sizeof(CancelOrderAccepted), MessageType::cancelOrderAccepted},
          exchangeOrderId(exchangeOrderId_) {}
    Xoid exchangeOrderId;
    std::string string() {
        std::stringstream s;
        s << "type=CancelOrderAccepted, xoid=" << exchangeOrderId;
        return s.str();
    }
};

struct CancelOrderRejected : MessageHeader {
    CancelOrderRejected(Xoid exchangeOrderId_)
        : MessageHeader{sizeof(CancelOrderRejected), MessageType::cancelOrderRejected},
          exchangeOrderId(exchangeOrderId_) {}
    Xoid exchangeOrderId;
    std::string string() {
        std::stringstream s;
        s << "type=CancelOrderRejected, xoid=" << exchangeOrderId;
        return s.str();
    }
};

struct Trade : MessageHeader {
    Trade(Volume volume_, Price price_, TradeId tradeId_, Xoid orderId_, Xoid counterOrderId_)
        : MessageHeader{sizeof(Trade), MessageType::trade},
          volume(volume_),
          price(price_),
          tradeId(tradeId_),
          orderId(orderId_),
          counterOrderId(counterOrderId_) {}

    Volume volume;
    Price price;
    TradeId tradeId;
    Xoid orderId;
    Xoid counterOrderId;

    std::string string() {
        std::stringstream s;
        s << "type=Trade, Volume=" << volume << ", price=" << price << ", orderId=" << orderId
          << ", cptyOrderId=" << counterOrderId;
        return s.str();
    }
};

#pragma pack(pop)

}  // namespace fun::client
