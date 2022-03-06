/*
 * Copyright (C) 2022
 */

#pragma once

#include <string>
#include <optional>
#include <vector>
#include <variant>

#include "tcp_socket.hpp"
#include "message_parser.hpp"
#include "types.hpp"
#include "timer.hpp"

namespace fun::client {

class ExpectReply {
    std::vector<Message> receivedMessages_;
    std::vector<Message> actualMessages_;
    std::vector<Message> messages_;
    MessageParser parser_;

public:
    ExpectReply(Receiving receiving) : parser_(receiving) {}
    void expectTrade(Volume volume, Price price, Xoid exchangeOrderId, Xoid cptyExchangeOrderId);
    void expectOrderAdded();
    void expectOrderCancelled(Xoid exchangeOrderId);
    void expectOrderCancelRejected(Xoid exchangeOrderId);

    // timeout period is set in the socket option.
    bool wait(Timer &timer);
    void clear();
    std::vector<Message> &getActualMessages() { return actualMessages_; }
    std::vector<Message> &getReceivedMessages() { return receivedMessages_; }

private:
    static bool compareEqual(const AddOrderAccepted &msg1, const AddOrderAccepted &msg2);
    static bool compareEqual(const CancelOrderAccepted &msg1, const CancelOrderAccepted &msg2);
    static bool compareEqual(const CancelOrderRejected &msg1, const CancelOrderRejected &msg2);
    static bool compareEqual(const Trade &msg1, const Trade &msg2);
};

class Client {
    Receiving receiving_;
    Sending sending_;

public:
    Client(Receiving receiving, Sending sending) : receiving_(receiving), sending_(sending) {}
    void addOrder(Side side, OrderType orderType, InstrumentId instrumentId, Price price, Volume volume);
    void cancelOrder(Xoid exchangeOrderId);
    Receiving getReceiving() { return receiving_; }
    Sending getSending() { return sending_; }
};

}  // namespace fun::client
