/*
 * Copyright (C) 2022
 */

#include <algorithm>
#include <numeric>
#include <vector>
#include <sstream>

#include "timer.hpp"
#include "logger.hpp"

using namespace std;
using namespace std::chrono;
using namespace fun::client;

void Timer::start() { start_ = high_resolution_clock::now(); }

void Timer::takeTime(TimerId timerId) {
    auto end = high_resolution_clock::now();
    auto timeTaken = duration_cast<nanoseconds>(end - start_);
    timings_[int(timerId)].emplace_back(timeTaken.count());
    start_ = end;
}

void dumpValues(const string &timerName, const vector<uint64_t> &values) {
    auto copy = vector<uint64_t>{values.begin(), values.end()};
    if (copy.empty()) {
        return;
    }
    sort(copy.begin(), copy.end());

    auto m = mean(copy);
    auto sdv = sd(values, m);
    auto i99 = m + 2.576 * sdv;
    SPDLOG_INFO("Timing for {}", timerName);
    SPDLOG_INFO("    mean = {:.0f} ns", m);
    SPDLOG_INFO("    sd = {:.0f} ns", sdv);
    SPDLOG_INFO("    upper 99% (mean + 2.576 * sd) = {:.0f} ns", i99);
    SPDLOG_INFO("    total num = {}", values.size());

    stringstream s;
    static const int PART = 20;
    for (int p = 0; p <= PART; ++p) {
        if (p > 0) {
            s << ", ";
        }
        size_t nth = p == PART ? (copy.size() - 1) : (copy.size() * p / PART);
        s << int(100 * (double(p) / PART)) << "% = " << copy[nth];
    }
    SPDLOG_INFO("    {}", s.str());
    auto p99 = size_t(0.99 * copy.size());
    SPDLOG_INFO("    99%-tile = {} ns", copy[p99]);
}

void Timer::dump() {
    for (int i = 0; i < timerNames_.size(); ++i) {
        if (timings_[i].empty()) {
            continue;
        }
        dumpValues(timerNames_[i], timings_[i]);
    }

    vector<uint64_t> all{};
    for (int i = 0; i < timerNames_.size(); ++i) {
        all.insert(all.end(), timings_[i].begin(), timings_[i].end());
    }
    dumpValues("Overall", all);
}

Timer::Timer()
    : timerNames_{vector<string>{"AddOrderAccepted", "CancelOrderRejected", "CancelOrderAccepted", "Trade"}} {
    timings_.resize(timerNames_.size());
    for (int i = 0; i < timerNames_.size(); ++i) {
        timings_[i].reserve(5000000);
    }
}
