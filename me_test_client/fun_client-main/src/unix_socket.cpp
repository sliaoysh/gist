/*
 * Copyright (C) 2022
 */

#include <sys/un.h>
#include <unistd.h>
#include <stdexcept>

#include "error.hpp"
#include "logger.hpp"
#include "unix_socket.hpp"

using namespace std;
using namespace fun::client;

void UnixSocket::connect(const string &target) {
    sockaddr_un address{};
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, target.c_str());

    int servlen = strlen(address.sun_path) + sizeof(address.sun_family);

    auto result = ::connect(socket_, (const sockaddr *)&address, servlen);
    if (result != 0) {
        throw runtime_error("connect failed");
    }
}

void UnixSocket::bind(const string &target) {
    sockaddr_un address{};
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, target.c_str());

    int servlen = strlen(address.sun_path) + sizeof(address.sun_family);
    auto result = ::bind(socket_, (const sockaddr *)&address, servlen);
    if (result == -1) {
        Error::throwException("bind failed");
    }
}

UnixSocket::UnixSocket() {
    socket_ = socket(AF_UNIX, SOCK_STREAM, 0);
    setDefaultOptions();
}

void UnixSocket::setDefaultOptions() {
    const timeval tv{60, 0};
    setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    const int opt = 1;
    setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    linger l{};
    setsockopt(socket_, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
}

UnixSocket UnixSocket::accept() { return UnixSocket(acceptImpl()); }
