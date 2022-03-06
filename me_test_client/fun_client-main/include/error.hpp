/*
 * Copyright (C) 2022
 */

#pragma once

#include <string>
#include <system_error>

class Error {
public:
    static std::string getErrnoMessage() {
        auto errorCode = std::make_error_code(static_cast<std::errc>(errno));
        return errorCode.message();
    }

    static void throwException(const std::string &msg) {
        throw std::runtime_error(msg + "[error = " + getErrnoMessage() + "]");
    }
};