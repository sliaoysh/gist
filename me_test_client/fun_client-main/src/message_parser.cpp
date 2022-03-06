/*
 * Copyright (C) 2022
 */

#include "message_parser.hpp"
#include "logger.hpp"

using namespace std;
using namespace fun::client;

template <typename T>
T *getMessage(MessageHeader &header, Byte *buffer) {
    if (header.size != sizeof(T) - sizeof(header.size)) {
        return nullptr;
    }
    return reinterpret_cast<T *>(buffer);
}

optional<Message> MessageParser::read() {
    auto len = receiving_(buffer_, 1);
    if (len <= 0) {
        return {};
    }
    uint8_t packetSize = buffer_[0];
    auto len1 = receiving_(buffer_ + 1, packetSize);
    if (!len1 || len1 != packetSize) {
        SPDLOG_INFO("MessageParser::read, received length={}, received packetSize={}", len1, packetSize);
        return {};
    }
    auto &header = *reinterpret_cast<MessageHeader *>(buffer_);
    switch (header.msgType) {
    case MessageType::addOrder:
        return *getMessage<AddOrder>(header, buffer_);
    case MessageType::addOrderAccepted:
        return *getMessage<AddOrderAccepted>(header, buffer_);
    case MessageType::cancelOrder:
        return *getMessage<CancelOrder>(header, buffer_);
    case MessageType::cancelOrderAccepted:
        return *getMessage<CancelOrderAccepted>(header, buffer_);
    case MessageType::cancelOrderRejected:
        return *getMessage<CancelOrderRejected>(header, buffer_);
    case MessageType::trade:
        return *getMessage<Trade>(header, buffer_);
    default:
        return {};
    }
}
