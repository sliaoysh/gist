/*
 * Copyright (C) 2022
 */

#pragma once

#include <chrono>
#include <cmath>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

namespace fun::client {

template <typename T>
double mean(T t) {
    return accumulate(t.begin(), t.end(), double(0)) / t.size();
}

template <typename T>
double sd(T t, double mean) {
    double sum = 0;
    for (auto i : t) {
        auto diff = double(i) - mean;
        sum += diff * diff;
    }
    return std::sqrt(sum / t.size());
}

enum class TimerId {
    AddOrderAccepted,
    CancelOrderRejected,
    CancelOrderAccepted,
    Trade,
};

class Timer {
    std::chrono::high_resolution_clock::time_point start_;
    std::vector<std::vector<uint64_t>> timings_;
    std::vector<std::string> timerNames_;

public:
    Timer();
    void start();
    void takeTime(TimerId timerId);
    void dump();
};

}  // namespace fun::client
