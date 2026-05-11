#pragma once

#include <chrono>
#include <functional>
#include <thread>
#include <atomic>

class Timer {
public:
    Timer();
    double elapsed_seconds() const;
    double elapsed_milliseconds() const;
    void reset();

private:
    std::chrono::steady_clock::time_point start_;
};

class RepeatingTimer {
public:
    RepeatingTimer(double interval_ms, std::function<void()> callback);
    ~RepeatingTimer();

    void start();
    void stop();
    bool is_running() const;

private:
    double interval_ms_;
    std::function<void()> callback_;
    std::atomic<bool> running_{false};
    std::thread thread_;
};
