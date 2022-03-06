/*
 * Copyright (C) 2022
 */

#pragma once

#include <functional>
#include <variant>

#include "message_types.hpp"

namespace fun::client {

using Message = std::variant<AddOrder, AddOrderAccepted, CancelOrder, CancelOrderAccepted, CancelOrderRejected, Trade>;
class MessageParser {
    static constexpr uint BUFFER_SIZE = 1 << (sizeof(MessageHeader::size) * 8);
    Receiving receiving_;
    Byte buffer_[BUFFER_SIZE]{};

public:
    MessageParser(Receiving receiving) : receiving_(receiving){};
    std::optional<Message> read();
};

}  // namespace fun::client
