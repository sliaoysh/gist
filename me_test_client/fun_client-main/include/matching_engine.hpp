/*
 * Copyright (C) 2022
 */

#pragma once

#include <string>
#include "message_types.hpp"
#include "order_book.hpp"
#include "tcp_socket.hpp"

using namespace fun::Matching;
namespace fun::client {
#define BUYSTR "B"
#define SYMBOL "A"
#define NCOL 6

class MatchingEngine {
    std::string address_;
    int port_{};
    TcpSocket socket_;

    OrderBook *m_orderBook;

public:
    MatchingEngine(const std::string &address, int port);
    virtual ~MatchingEngine() { clean(); }

    [[noreturn]] void start();
    void acceptClient();
    void handleRequest(TcpSocket &clientSocket, const AddOrder &addOrder);
    void handleRequest(TcpSocket &clientSocket, const CancelOrder &cancelOrder);

    void clean() { delete m_orderBook; }
    void processOrder(Order *order);

    // We can remove this later.
    int run(const string &inFile);
    void init(const vector<string> &names);
};

}  // namespace fun::client
