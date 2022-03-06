/*
 * Copyright (C) 2022
 */

#pragma once

#include <spdlog/spdlog.h>

namespace fun::client {

struct Logger {
    Logger() {
        spdlog::set_pattern("%Y-%m-%d|%H:%M:%S|%z|%P|%t|%n|%l|%v");
    }
};

}  // namespace fun::client
