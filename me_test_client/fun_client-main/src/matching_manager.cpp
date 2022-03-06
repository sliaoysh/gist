/*
 * Copyright (C) 2022
 */

#include "matching_manager.hpp"

using namespace std;
using namespace fun::client;

void MatchingManager::setClientSocket(std::reference_wrapper<TcpSocket> socket) { clientSocket_ = socket; }

void MatchingManager::addOrder(const AddOrder &addOrder) {}

void MatchingManager::cancelOrder(const CancelOrder &cancelOrder) {}
