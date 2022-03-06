/*
 * Copyright (C) 2022
 */

#pragma once

#include <functional>

namespace fun::client {

class Final {
    std::function<void(void)> cleanup_;

public:
    Final(std::function<void(void)> cleanup) : cleanup_(cleanup) {}
    ~Final();
};

}  // namespace fun::client
