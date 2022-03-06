/*
 * Copyright (C) 2022
 */

#include <unistd.h>
#include <stdexcept>

#include "error.hpp"
#include "logger.hpp"
#include "tcp_socket.hpp"

using namespace std;
using namespace fun::client;

ssize_t TcpSocket::send(const Byte *buffer, uint32_t length) { return ::send(socket_, buffer, length, 0); }

ssize_t TcpSocket::receive(Byte *buffer, uint32_t length) { return ::recv(socket_, buffer, length, 0); }

void TcpSocket::connect(const string &ip, uint16_t port) {
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.data(), &serverAddress.sin_addr) <= 0) {
        throw runtime_error("inet_pton failed");
    }
    auto result = ::connect(socket_, (const sockaddr *)&serverAddress, sizeof(serverAddress));
    if (result != 0) {
        throw runtime_error("connect failed");
    }
}

void TcpSocket::bind(const string &ip, uint16_t port) {
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.data(), &address.sin_addr) <= 0) {
        Error::throwException("inet_pton failed");
    }

    auto result = ::bind(socket_, (const sockaddr *)&address, sizeof(address));
    if (result == -1) {
        Error::throwException("bind failed");
    }
}

void TcpSocket::listen() {
    auto result = ::listen(socket_, 1);
    if (result) {
        Error::throwException("error in listening");
    }
}

int TcpSocket::acceptImpl() {
    sockaddr_in client_address;
    socklen_t length = sizeof(client_address);
    int fd = ::accept(socket_, (sockaddr *)&client_address, &length);
    if (fd < 0) {
        Error::throwException("cannot accept");
    }
    return fd;
}

TcpSocket TcpSocket::accept() { return TcpSocket(acceptImpl()); }

TcpSocket::TcpSocket(int fd) : socket_(fd) { setDefaultOptions(); }

TcpSocket::TcpSocket() {
    socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    setDefaultOptions();
}

void TcpSocket::setDefaultOptions() {
    const timeval tv{60, 0};
    setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    const int opt = 1;
    setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
    setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    linger l{};
    setsockopt(socket_, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
}

TcpSocket::TcpSocket(TcpSocket &&sock) noexcept : TcpSocket(sock.socket_) { sock.socket_ = 0; }

TcpSocket &TcpSocket::operator=(TcpSocket &&other) noexcept {
    if (&other == this) {
        return *this;
    }
    this->~TcpSocket();
    new (this) TcpSocket(std::move(other));
    return *this;
}

TcpSocket::~TcpSocket() {
    SPDLOG_DEBUG("destroying socket, fd = {}", socket_);
    if (socket_ > 0) {
        auto ret = ::close(socket_);
        if (ret == -1) {
            SPDLOG_ERROR(Error::getErrnoMessage());
        }
    }
}

Sending TcpSocket::getSending() {
    return [&](const Byte *buffer, size_t length) { return send(buffer, length); };
}

Receiving TcpSocket::getReceiving() {
    return [&](Byte *buffer, size_t length) { return receive(buffer, length); };
}
