/*
 * Copyright (C) 2022
 */

#include <catch2/catch.hpp>
#include <thread>
#include <future>

#include "feed_player.hpp"
#include "tcp_socket.hpp"
#include "logger.hpp"
#include "final.hpp"

using namespace fun::client;
using namespace std;

FeedMessage getFeedMessage(MessageType action, Coid orderId = 0, InstrumentId instrumentId = 0, Volume volume = 0,
                           Price price = 0, bool isBuy = false, Coid counterOrderId = 0) {
    return FeedMessage::create(action, orderId, instrumentId, volume, price, isBuy, counterOrderId);
}

void createServer(promise<string> &result, promise<bool> &ready, const string &address, int port, int length) {
    TcpSocket sock;
    sock.bind(address, port);
    sock.listen();
    ready.set_value(true);
    auto clientSock = sock.accept();

    Byte buffer[length];
    clientSock.receive(buffer, length);
    string output((char *)buffer, length);
    result.set_value(output);
}

TEST_CASE("TcpSocket") {
    static const string address = "127.0.0.1";
    static const int port = 8001;
    static const string expectation = "Hello world!";
    promise<string> result;
    promise<bool> ready;
    thread t([&]() { createServer(result, ready, address, port, expectation.size()); });
    Final final([&] { t.join(); });

    TcpSocket sock;
    REQUIRE(ready.get_future().get());
    sock.connect(address, port);
    sock.send((Byte *)expectation.data(), expectation.size());
    REQUIRE(result.get_future().get() == expectation);
}

TEST_CASE("calculation") {
    vector<int> x = {1, 2, 3, 4, 5, 9};
    REQUIRE(mean(x) == 4);
    REQUIRE(abs(sd(x, mean(x)) - 2.58199) < 0.00001);
}

TEST_CASE("FeedPlayer") {
    static InstrumentId id1 = 1000003;
    static TradeId tid = 5000;
    FeedPlayer player;
    list<FeedMessage> feedMessages = {
        getFeedMessage(MessageType::addOrder, 2770, id1, 450, 40300, false),
        getFeedMessage(MessageType::addOrderAccepted, 2770),
        getFeedMessage(MessageType::addOrder, 2771, id1, 450, 40300, false),
        getFeedMessage(MessageType::addOrderAccepted, 2771),
        getFeedMessage(MessageType::addOrder, 2772, id1, 450, 40300, false),
        getFeedMessage(MessageType::trade, 2772, id1, 450, 40300, false, 2771),
        getFeedMessage(MessageType::trade, 2771, id1, 450, 40300, true, 2772),
        getFeedMessage(MessageType::addOrderAccepted, 2772),
        getFeedMessage(MessageType::cancelOrder, 2770),
        getFeedMessage(MessageType::cancelOrderAccepted, 2770),
        getFeedMessage(MessageType::cancelOrder, 2775),
        getFeedMessage(MessageType::cancelOrderRejected, 2775),
    };

    stringstream output, input;
    Receiving receiving = [&](Byte *buffer, size_t length) {
        output.read((char *)buffer, length);
        return length;
    };

    Sending sending = [&](const Byte *buffer, size_t length) {
        input.write((char *)buffer, length);
        return length;
    };

    AddOrderAccepted accepted(100), accepted1(101), accepted2(102);
    CancelOrderAccepted cancelAccepted(100);
    CancelOrderRejected cancelOrderRejected(2775);
    Trade trade(450, 40300, tid, 101, 102), trade1(450, 40300, tid, 102, 101);
    auto writeOutput = [&](auto &msg) { output.write((char *)&msg, sizeof(msg)); };
    writeOutput(accepted);
    writeOutput(accepted1);
    writeOutput(trade);
    writeOutput(trade1);
    writeOutput(accepted2);
    writeOutput(cancelAccepted);
    writeOutput(cancelOrderRejected);
    Client client(receiving, sending);
    player.play(client, feedMessages);
    auto orderMapping = player.getOrderMapping();
    REQUIRE(orderMapping[2770] == 100);
    REQUIRE(orderMapping[2771] == 101);
    REQUIRE(orderMapping[2772] == 102);
    player.dumpTiming();
}
