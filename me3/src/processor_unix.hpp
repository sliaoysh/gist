#pragma once

#include "matching_engine.hpp"
#include "server_socket_unix.hpp"

class processor
{
public:
    processor(const char* sock_name, const std::vector<fun::client::InstrumentId>& instruments);
    void run();

private:
    matching_engine me_;
    server_socket_unix soc_;
    std::vector<fun::client::Trade> trades_;
};
