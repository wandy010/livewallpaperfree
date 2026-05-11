#include "timers.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

// -- Timer ----------------------------------------------------------------

Timer::Timer()
    : start_(std::chrono::steady_clock::now()) {}

double Timer::elapsed_seconds() const {
    auto now = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::duration<double>>(now - start_);
    return diff.count();
}

double Timer::elapsed_milliseconds() const {
    return elapsed_seconds() * 1000.0;
}

void Timer::reset() {
    start_ = std::chrono::steady_clock::now();
}

// -- RepeatingTimer -------------------------------------------------------

RepeatingTimer::RepeatingTimer(double interval_ms, std::function<void()> callback)
    : interval_ms_(interval_ms), callback_(std::move(callback)) {}

RepeatingTimer::~RepeatingTimer() {
    stop();
}

void RepeatingTimer::start() {
    if (running_) return;
    running_ = true;
    thread_ = std::thread([this]() {
        auto interval = std::chrono::duration<double, std::milli>(interval_ms_);
        while (running_) {
            std::this_thread::sleep_for(interval);
            if (running_ && callback_) {
                callback_();
            }
        }
    });
}

void RepeatingTimer::stop() {
    running_ = false;
    if (thread_.joinable()) {
        thread_.join();
    }
}

bool RepeatingTimer::is_running() const {
    return running_;
}
