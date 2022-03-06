/*
 * Copyright (C) 2022
 */

#include "feed_player.hpp"
#include <variant>
#include "spdlog/spdlog.h"

using namespace fun::client;
using namespace std;

list<FeedMessage> FeedPlayer::process_csv(string fileName) {
    // Reads a CSV file into a list of std::list<FeedMessage>s

    // Create a list of FeedMessage to  store the result
    list<FeedMessage> result;

    // Create an input filestream
    ifstream myFile(fileName);

    // Make sure the file is open
    if (!myFile.is_open()) throw std::runtime_error("Could not open file");

    // Helper vars
    string line, colname;

    // Read the column names
    if (myFile.good()) {
        // Extract the first line in the file
        std::getline(myFile, line);

        // Create a string stream from line
        std::stringstream ss(line);

        // Extract each column name
        //            while (std::getline(ss, colname, ',')) {
        //                // Initialize and add <colname, int vector> pairs to result
        //                result.insert({colname, std::vector<std::string>{}});
        //            }
    }

    // Read data, line by line
    while (getline(myFile, line)) {
        // Create a string stream of the current line
        std::stringstream ss(line);
        std::string value;

        FeedMessage feedMessage{};

        // Keep track of the current column index
        int colIdx = 0;

        while (std::getline(ss, value, ',')) {
            if (!value.empty()) {
                switch (colIdx) {
                case MESSAGE_TYPE_INDEX:
                    feedMessage.action = value;
                    break;
                case ORDER_ID_INDEX:
                    feedMessage.orderId = std::stoul(value);
                    break;
                case INSTRUMENT_INDEX:
                    feedMessage.instrumentId = std::stoi(value);
                    break;
                case VOLUME_INDEX:
                    feedMessage.volume = std::stoul(value);
                    break;
                case PRICE_INDEX:
                    feedMessage.price = std::stoul(value);
                    break;
                case SIDE_INDEX:
                    feedMessage.isBuy = value == "B";
                    break;
                case C_ORDER_ID_INDEX:
                    feedMessage.counterOrderId = std::stoul(value);
                    break;

                default:
                    ss.ignore();
                    break;
                }
            }
            // Increment the column index
            colIdx++;
        }
        result.emplace_back(feedMessage);
    }

    // Close file
    myFile.close();

    return result;
}

void FeedPlayer::expectSend(ExpectReply &expecter, Client &client, FeedMessage &action) {
    auto actionType = static_cast<MessageType>(action.action[0]);
    timer_.start();
    switch (actionType) {
    case MessageType::addOrder: {
        auto side = action.isBuy ? Side::buy : Side::sell;
        SPDLOG_DEBUG("sending add order, coid={}", action.orderId);
        client.addOrder(side, OrderType::gtc, action.instrumentId, action.price, action.volume);
        coid_ = action.orderId;
        state_ = PlayerState::expectAddOrderReply;
        break;
    }
    case MessageType::cancelOrder: {
        auto it = orderMapping_.find(action.orderId);
        xoid_ = it == orderMapping_.end() ? action.orderId : it->second;
        SPDLOG_DEBUG("sending cancel order, coid={}, xoid={}", action.orderId, xoid_);
        client.cancelOrder(xoid_);
        state_ = PlayerState::expectCancelOrderReply;
        break;
    }
    default:
        SPDLOG_ERROR("expectSend: Unexpected action = {}", action.action);
        throw runtime_error("Unexpected action");
    }
    expecter.clear();
}

void FeedPlayer::expectAddOrderReply(ExpectReply &expecter, Client &client, FeedMessage &action) {
    auto actionType = static_cast<MessageType>(action.action[0]);
    switch (actionType) {
    case MessageType::addOrderAccepted:
        expecter.expectOrderAdded();
        waitForReply(expecter, client);
        state_ = PlayerState::expectSend;
        break;
    case MessageType::trade: {
        auto it = orderMapping_.find(action.orderId);
        auto orderId = it == orderMapping_.end() ? nullXoid : it->second;
        auto it1 = orderMapping_.find(action.counterOrderId);
        auto counterOrderId = it1 == orderMapping_.end() ? nullXoid : it1->second;
        expecter.expectTrade(action.volume, action.price, orderId, counterOrderId);
        break;
    }
    default:
        SPDLOG_ERROR("expectCancelOrderReply: Unexpected action = {}", action.action);
        throw runtime_error("Unexpected action");
    }
}

void FeedPlayer::expectCancelOrderReply(ExpectReply &expecter, Client &client, FeedMessage &action) {
    auto actionType = static_cast<MessageType>(action.action[0]);
    switch (actionType) {
    case MessageType::cancelOrderAccepted:
        expecter.expectOrderCancelled(xoid_);
        break;
    case MessageType::cancelOrderRejected:
        expecter.expectOrderCancelRejected(xoid_);
        break;
    default:
        SPDLOG_ERROR("expectCancelOrderReply: Unexpected action = {}", action.action);
        throw runtime_error("Unexpected action");
    }

    waitForReply(expecter, client);
    state_ = PlayerState::expectSend;
}

void FeedPlayer::play(Client &client, std::list<FeedMessage> feedMessages) {
    ExpectReply expecter(client.getReceiving());
    for (auto &action : feedMessages) {
        switch (state_) {
        case PlayerState::expectSend:
            expectSend(expecter, client, action);
            break;
        case PlayerState::expectAddOrderReply:
            expectAddOrderReply(expecter, client, action);
            break;
        case PlayerState::expectCancelOrderReply:
            expectCancelOrderReply(expecter, client, action);
            break;
        }
    }
}

void FeedPlayer::waitForReply(ExpectReply &expecter, Client &client) {
    SPDLOG_DEBUG("expecting reply");
    auto ret = expecter.wait(timer_);
    if (!ret) {
        throw runtime_error("Message expecting failed");
    }
    auto actualMessages = expecter.getActualMessages();
    auto *last = get_if<AddOrderAccepted>(&actualMessages.back());
    if (last) {
        orderMapping_[coid_] = last->exchangeOrderId;
    }
}

void FeedPlayer::dumpTiming() { timer_.dump(); }
