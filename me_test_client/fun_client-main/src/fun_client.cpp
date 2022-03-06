/*
 * Copyright (C) 2022
 */

#include "fun_client.hpp"
#include "message_types.hpp"
#include "spdlog/spdlog.h"

using namespace fun::client;
using namespace std;

template <typename T>
bool NonZeroEqual(T t1, T t2) {
    return !t1 || !t2 || t1 == t2;
}

void ExpectReply::expectTrade(Volume volume, Price price, Xoid exchangeOrderId, Xoid cptyExchangeOrderId) {
    messages_.emplace_back(Trade{volume, price, nullTradeId, exchangeOrderId, cptyExchangeOrderId});
}

void ExpectReply::expectOrderAdded() { messages_.emplace_back(AddOrderAccepted{nullXoid}); }

void ExpectReply::expectOrderCancelled(Xoid exchangeOrderId) {
    messages_.emplace_back(CancelOrderAccepted{exchangeOrderId});
}

void ExpectReply::expectOrderCancelRejected(Xoid exchangeOrderId) {
    messages_.emplace_back(CancelOrderRejected{exchangeOrderId});
}

bool ExpectReply::compareEqual(const AddOrderAccepted &msg1, const AddOrderAccepted &msg2) {
    return NonZeroEqual(msg1.exchangeOrderId, msg2.exchangeOrderId);
}

bool ExpectReply::compareEqual(const CancelOrderAccepted &msg1, const CancelOrderAccepted &msg2) {
    return NonZeroEqual(msg1.exchangeOrderId, msg2.exchangeOrderId);
}

bool ExpectReply::compareEqual(const CancelOrderRejected &msg1, const CancelOrderRejected &msg2) {
    return NonZeroEqual(msg1.exchangeOrderId, msg2.exchangeOrderId);
}

bool ExpectReply::compareEqual(const Trade &msg1, const Trade &msg2) {
    return NonZeroEqual(msg1.orderId, msg2.orderId) && NonZeroEqual(msg1.counterOrderId, msg2.counterOrderId) &&
           NonZeroEqual(msg1.price, msg2.price) && NonZeroEqual(msg1.volume, msg2.volume);
}

bool ExpectReply::wait(Timer &timer) {
    while (true) {
        auto message = parser_.read();
        if (!message) {
            SPDLOG_INFO("Failed to receive a valid message");
            return false;
        }
        receivedMessages_.emplace_back(*message);
        auto ret = visit(
            [&](auto msg) -> bool {
                using T = decltype(msg);
                if constexpr (is_same_v<T, AddOrder> || is_same_v<T, CancelOrder>) {
                    throw runtime_error("Unsupported message type");
                } else {
                    if constexpr (is_same_v<T, Trade>) {
                        SPDLOG_DEBUG("Received a trade, orderId={}, counterId={}, tradeId={}, volume={}, price={}",
                                     msg.orderId, msg.counterOrderId, msg.tradeId, msg.volume, msg.price);
                        timer.takeTime(TimerId::Trade);
                        return false;
                    } else if constexpr (is_same_v<T, AddOrderAccepted>) {
                        SPDLOG_DEBUG("Received an AddOrderAccepted, xoid={}", msg.exchangeOrderId);
                        timer.takeTime(TimerId::AddOrderAccepted);
                        return true;
                    } else if constexpr (is_same_v<T, CancelOrderAccepted>) {
                        SPDLOG_DEBUG("Received an CancelOrderAccepted, xoid={}", msg.exchangeOrderId);
                        timer.takeTime(TimerId::CancelOrderAccepted);
                        return true;
                    } else if constexpr (is_same_v<T, CancelOrderRejected>) {
                        SPDLOG_DEBUG("Received an CancelOrderRejected, xoid={}", msg.exchangeOrderId);
                        timer.takeTime(TimerId::CancelOrderRejected);
                        return true;
                    }
                }
            },
            *message);

        if (ret) {
            break;
        }
    }

    if (messages_.empty()) {
        // nothing needs to be expected. So return true (should be the recording mode)
        return true;
    }

    if (receivedMessages_.size() != messages_.size()) {
        SPDLOG_ERROR("Wrong number of messages expected. expecting: {}, actual: {}", messages_.size(),
                     receivedMessages_.size());
        return false;
    }

    for (int i = 0; i < messages_.size(); ++i) {
        bool matched = visit(
            [&](auto msg) {
                using T = decltype(msg);
                bool matched = false;
                for (auto &received : receivedMessages_) {
                    auto *receivedMsg = get_if<T>(&received);
                    if (!receivedMsg) {
                        continue;
                    }
                    if constexpr (is_same_v<T, AddOrder> || is_same_v<T, CancelOrder>) {
                        // checked before. Just to make the compiler happy.
                        throw runtime_error("Unsupported message type");
                    } else if (compareEqual(msg, *receivedMsg)) {
                        matched = true;
                        actualMessages_.emplace_back(*receivedMsg);
                        break;
                    }
                }
                if (!matched) {
                    SPDLOG_ERROR("Unable to expect: {}", msg.string());
                }
                return matched;
            },
            messages_[i]);
        if (!matched) {
            return false;
        }
    }

    return true;
}

void ExpectReply::clear() {
    actualMessages_.clear();
    messages_.clear();
    receivedMessages_.clear();
}

void Client::addOrder(Side side, OrderType orderType, InstrumentId instrumentId, Price price, Volume volume) {
    AddOrder addOrder{
        side, orderType, instrumentId, volume, price,
    };
    sending_((const Byte *)&addOrder, sizeof(AddOrder));
}

void Client::cancelOrder(Xoid exchangeOrderId) {
    CancelOrder cancelOrder{
        exchangeOrderId,
    };
    sending_((const Byte *)&cancelOrder, sizeof(CancelOrder));
}
