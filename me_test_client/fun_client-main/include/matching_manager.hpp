/*
 * Copyright (C) 2022
 */

#pragma once

#include <functional>
#include "message_types.hpp"
#include "tcp_socket.hpp"

namespace fun::client {

class MatchingManager {
    std::reference_wrapper<TcpSocket> clientSocket_;

public:
    MatchingManager();
    void setClientSocket(std::reference_wrapper<TcpSocket> socket);
    void addOrder(const AddOrder &addOrder);
    void cancelOrder(const CancelOrder &cancelOrder);
};

}  // namespace fun::client