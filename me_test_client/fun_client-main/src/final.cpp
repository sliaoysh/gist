/*
 * Copyright (C) 2022
 */

#include "final.hpp"
#include "logger.hpp"

using namespace fun::client;
using namespace std;

Final::~Final() {
    if (cleanup_) {
        try {
            cleanup_();
        } catch (const std::exception &e) {
            SPDLOG_DEBUG("exception in Final destructor = {}", e.what());
        } catch (...) {
            SPDLOG_DEBUG("exception in Final destructor");
        }
    }
}