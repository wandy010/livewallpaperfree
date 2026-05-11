#include "logger.h"

#include <iostream>
#include <ctime>
#include <iomanip>

Logger::~Logger() {
    if (file_stream_.is_open()) {
        file_stream_.close();
    }
}

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::set_level(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    current_level_ = level;
}

void Logger::set_log_file(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (file_stream_.is_open()) {
        file_stream_.close();
    }
    file_stream_.open(path, std::ios::app);
}

void Logger::debug(const std::string& message) {
    instance().log_impl(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    instance().log_impl(LogLevel::INFO, message);
}

void Logger::warn(const std::string& message) {
    instance().log_impl(LogLevel::WARN, message);
}

void Logger::error(const std::string& message) {
    instance().log_impl(LogLevel::ERR, message);
}

void Logger::log_impl(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (level < current_level_) return;

    std::string line = "[" + level_to_string(level) + "] "
                     + "[" + current_timestamp() + "] "
                     + message;

    std::ostream& out = (level >= LogLevel::WARN) ? std::cerr : std::cout;
    out << line << std::endl;

    if (file_stream_.is_open()) {
        file_stream_ << line << std::endl;
    }
}

std::string Logger::level_to_string(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERR: return "ERROR";
        default:              return "UNKNOWN";
    }
}

std::string Logger::current_timestamp() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
