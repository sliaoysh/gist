/*
 * Copyright (C) 2022
 */

#pragma once

#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstdint>
#include <optional>
#include <string>

#include "message_parser.hpp"
#include "types.hpp"

namespace fun::client {

class TcpSocket {
    void setDefaultOptions();

protected:
    int socket_{};
    int acceptImpl();

public:
    TcpSocket();
    TcpSocket(int fd);
    TcpSocket(TcpSocket &) = delete;
    TcpSocket &operator=(TcpSocket &) = delete;
    TcpSocket(TcpSocket &&) noexcept;
    TcpSocket &operator=(TcpSocket &&) noexcept;

    ssize_t send(const Byte *buffer, uint32_t length);
    ssize_t receive(Byte *buffer, uint32_t length);
    Sending getSending();
    Receiving getReceiving();
    void connect(const std::string &ip, uint16_t port);
    void bind(const std::string &ip, uint16_t port);
    void listen();
    TcpSocket accept();

    ~TcpSocket();
};

}  // namespace fun::client
