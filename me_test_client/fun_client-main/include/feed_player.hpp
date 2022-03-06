/*
 * Copyright (C) 2022
 */

#pragma once

#include <cstdint>
#include <fstream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "timer.hpp"
#include "message_types.hpp"
#include "fun_client.hpp"

namespace fun::client {

const int MESSAGE_TYPE_INDEX = 0;
const int ORDER_ID_INDEX = 1;
const int INSTRUMENT_INDEX = 2;
const int VOLUME_INDEX = 3;
const int PRICE_INDEX = 4;
const int SIDE_INDEX = 5;
const int C_ORDER_ID_INDEX = 6;

struct FeedMessage {
    std::string action;
    Coid orderId;
    InstrumentId instrumentId;
    Volume volume;
    Price price;
    bool isBuy;
    Coid counterOrderId = 0;
    static FeedMessage create(MessageType action, Coid orderId = 0, InstrumentId instrumentId = 0, Volume volume = 0,
                              Price price = 0, bool isBuy = false, Coid counterOrderId = 0) {
        return FeedMessage{std::string{char(action)}, orderId, instrumentId, volume, price, isBuy, counterOrderId};
    }
};

class FeedPlayer {
protected:
    enum class PlayerState {
        expectSend,
        expectAddOrderReply,
        expectCancelOrderReply,
    };

    PlayerState state_{};
    Coid coid_{};
    Xoid xoid_{};
    std::unordered_map<Coid, Xoid> orderMapping_;
    Timer timer_;

public:
    std::list<FeedMessage> process_csv(std::string fileName);
    void play(Client &client, std::list<FeedMessage> feedMessages);

    void expectSend(ExpectReply &expecter, Client &client, FeedMessage &action);
    void expectAddOrderReply(ExpectReply &expecter, Client &client, FeedMessage &action);
    void expectCancelOrderReply(ExpectReply &expecter, Client &client, FeedMessage &action);
    void waitForReply(ExpectReply &expecter, Client &client);

    void dumpTiming();
    std::unordered_map<Coid, Xoid> getOrderMapping() { return orderMapping_; }
};

}  // namespace fun::client
