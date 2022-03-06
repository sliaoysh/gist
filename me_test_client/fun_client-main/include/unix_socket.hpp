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
#include "tcp_socket.hpp"
#include "types.hpp"

namespace fun::client {

class UnixSocket : public TcpSocket {
    void setDefaultOptions();

public:
    using TcpSocket::TcpSocket;

    UnixSocket();
    void connect(const std::string &unixSocketFile);
    void bind(const std::string &unixSocketFile);
    UnixSocket accept();
};

}  // namespace fun::client
