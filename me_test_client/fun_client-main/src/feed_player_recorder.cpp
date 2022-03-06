/*
 * Copyright (C) 2022
 */

#include "feed_player_recorder.hpp"
#include "logger.hpp"

using namespace fun::client;
using namespace std;

FeedPlayerRecorder::FeedPlayerRecorder() {}

class FieldWriter {
    ofstream &os_;

public:
    FieldWriter(ofstream &os) : os_(os) {}
    template <typename T>
    FieldWriter &write(T t) {
        os_ << ",";
        if (t) {
            os_ << t;
        }
        return *this;
    }
    template <typename T>
    FieldWriter &write(optional<T> t) {
        os_ << ",";
        if (t) {
            os_ << *t;
        }
        return *this;
    }
};

void writeLine(ofstream &recorded, const FeedMessage &message) {
    recorded << message.action[0];

    optional<char> isBuy;
    if (message.action[0] == char(MessageType::addOrder)) {
        isBuy = message.isBuy ? 'B' : 'S';
    }
    FieldWriter(recorded)
        .write(message.orderId)
        .write(message.instrumentId)
        .write(message.volume)
        .write(message.price)
        .write(isBuy)
        .write(message.counterOrderId);
    recorded << endl;
}

void FeedPlayerRecorder::play(Client &client, std::list<FeedMessage> feedMessages, const string &recordedFile) {
    ofstream recorded(recordedFile);
    recorded << "messageType,order_id,instrument,volume,price,side,counterOrderId" << endl;
    ExpectReply expecter(client.getReceiving());
    for (auto &action : feedMessages) {
        auto actionType = static_cast<MessageType>(action.action[0]);
        timer_.start();
        switch (actionType) {
        case MessageType::addOrder: {
            auto side = action.isBuy ? Side::buy : Side::sell;
            SPDLOG_DEBUG("sending add order, coid={}", action.orderId);
            client.addOrder(side, OrderType::gtc, action.instrumentId, action.price, action.volume);
            coid_ = action.orderId;
            writeLine(recorded, action);
            break;
        }
        case MessageType::cancelOrder: {
            auto it = orderMapping_.find(action.orderId);
            xoid_ = it == orderMapping_.end() ? action.orderId : it->second;
            SPDLOG_DEBUG("sending cancel order, coid={}, xoid={}", action.orderId, xoid_);
            client.cancelOrder(xoid_);
            writeLine(recorded, action);
            break;
        }
        default:
            continue;
        }
        expecter.clear();
        SPDLOG_DEBUG("expecting reply");
        expecter.wait(timer_);
        auto receivedMessages = expecter.getReceivedMessages();
        auto *last = get_if<AddOrderAccepted>(&receivedMessages.back());
        if (last) {
            orderMapping_[coid_] = last->exchangeOrderId;
            orderReverseMapping_[last->exchangeOrderId] = coid_;
        }
        for (auto message : receivedMessages) {
            visit(
                [&](auto msg) {
                    using T = decltype(msg);
                    if constexpr (is_same_v<T, AddOrder> || is_same_v<T, CancelOrder>) {
                        throw runtime_error("Unexpected return type");
                    } else if constexpr (is_same_v<T, AddOrderAccepted>) {
                        auto coid = orderReverseMapping_[msg.exchangeOrderId];
                        writeLine(recorded, FeedMessage::create(MessageType::addOrderAccepted, coid));
                    } else if constexpr (is_same_v<T, CancelOrderAccepted>) {
                        auto coid = orderReverseMapping_[msg.exchangeOrderId];
                        writeLine(recorded, FeedMessage::create(MessageType::cancelOrderAccepted, coid));
                    } else if constexpr (is_same_v<T, CancelOrderRejected>) {
                        writeLine(recorded, FeedMessage::create(MessageType::cancelOrderRejected, action.orderId));
                    } else if constexpr (is_same_v<T, Trade>) {
                        auto coid1 = orderReverseMapping_[msg.orderId];
                        auto coid2 = orderReverseMapping_[msg.counterOrderId];
                        writeLine(recorded, FeedMessage::create(MessageType::trade, coid1, 0, msg.volume, msg.price,
                                                                false, coid2));
                    }
                },
                message);
        }
    }
}