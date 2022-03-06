/*
 * Copyright (C) 2022
 */

#include "matching_engine.hpp"
#include <cstring>
#include "feed_player.hpp"
#include "message_parser.hpp"

using namespace std;
using namespace fun::client;

MatchingEngine::MatchingEngine(const string &address, int port) : address_(address), port_(port) {
    m_orderBook = new OrderBook();
    vector<string> names{SYMBOL};
    init(names);
}

void MatchingEngine::init(const vector<string> &names) { m_orderBook->bookTradeForInstrument(names); }

[[noreturn]] void MatchingEngine::start() {
    socket_.bind(address_, port_);
    socket_.listen();
    while (true) {
        acceptClient();
    }
}

void MatchingEngine::acceptClient() {
    auto clientSocket = socket_.accept();
    MessageParser parser([&](Byte *buffer, size_t length) { return clientSocket.receive(buffer, length); });

    while (true) {
        auto message = parser.read();
        if (!message) {
            break;
        }
        visit(
            [&](auto msg) {
                using T = decltype(msg);
                if constexpr (is_same_v<T, AddOrder> || is_same_v<T, CancelOrder>) {
                    handleRequest(clientSocket, msg);
                } else {
                    throw runtime_error("Unsupported message type");
                }
            },
            *message);
    }
}

void MatchingEngine::handleRequest(TcpSocket &clientSocket, const AddOrder &addOrder) {
    Volume qtyToMatch = addOrder.volume;

    Xoid orderId = 1;

    //        Order order = Order(orderId, )
    //
    //        // only match marketable order, qtyToMatch is the residual need to post on return
    //        m_orderBook->match(order, qtyToMatch);
    //
    //        // post non marketable portion
    //        if (qtyToMatch > 0) {
    //            if (qtyToMatch != order->m_quantity) order->m_quantity = qtyToMatch;
    //            m_orderBook->add(order);
    //        }
}
void MatchingEngine::handleRequest(TcpSocket &clientSocket, const CancelOrder &cancelOrder) {}

void MatchingEngine::processOrder(Order *order) {
    auto qtyToMatch = order->m_quantity;

    // only match marketable order, qtyToMatch is the residual need to post on return
    m_orderBook->match(order, qtyToMatch);

    // post non marketable portion
    if (qtyToMatch > 0) {
        if (qtyToMatch != order->m_quantity) order->m_quantity = qtyToMatch;
        m_orderBook->add(order);
    }
}

// implementation for reading it from file. We can delete this later.

int MatchingEngine::run(const string &inFile) {
    FeedPlayer player{};

    auto feedMessages = player.process_csv(inFile);
    Time timestamp = 12345678;
    Order *order;
    int t = 0;

    for (auto &message : feedMessages) {
        auto actionType = static_cast<MessageType>(message.action[0]);
        switch (actionType) {
        case MessageType::addOrder:
            order = new Order(message.orderId, SYMBOL, message.price, message.volume, timestamp, message.isBuy);
            processOrder(order);
            break;
        case MessageType::cancelOrder:
            t = 1;
            break;
        default:
            t = 0;
            break;
        }
    }

    return 0;
}
